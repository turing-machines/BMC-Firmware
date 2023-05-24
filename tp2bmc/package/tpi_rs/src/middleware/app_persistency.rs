use anyhow::bail;
use anyhow::Context;
use log::info;
use sqlx::Row;
use sqlx::SqlitePool;
use std::path::PathBuf;

/// directory on target which the sqlite database is written to.
const DATABASE: &str = concat!("/var/lib/", env!("CARGO_PKG_NAME"), "/bmc.db");

const VERSION: u32 = 1;

/// [ApplicationPersistency] is a simple key-value store that stores application state. Decided is
/// to go for a sqlite implementation. Its stable and there are enough tools available for it.
/// Preferably we would like to have a binary representation that we could sync to disk. At the
/// moment of writing are the current available crates not convincing enough to go down the route
/// of having a propiatary memory layout. The downside is that sqlite's size on flash is more
/// significant.
///
/// Databases are versioned with sqlite's user_version property. When you break the format, make
/// sure to implement a migration path and update the VERSION number.
#[derive(Debug)]
pub struct ApplicationPersistency {
    connection: SqlitePool,
}

impl ApplicationPersistency {
    /// Initializes a new instance, It will consruct a new database if it does not exist. Throws an
    /// error when the database version is not supported.
    pub async fn new() -> anyhow::Result<Self> {
        let path = PathBuf::from(DATABASE);
        let sql_db = format!("sqlite://{}", DATABASE);

        let connection = if !path.exists() {
            info!(
                "Database {} does not exist. creating new one..",
                path.to_string_lossy()
            );
            if let Some(p) = path.parent() {
                tokio::fs::create_dir_all(p).await?;
                tokio::fs::File::create(path).await?;
            }
            Self::setup_new(&sql_db).await?
        } else {
            Self::connect(&sql_db).await?
        };

        Ok(Self { connection })
    }

    async fn connect(db: &str) -> anyhow::Result<SqlitePool> {
        let connection = SqlitePool::connect(db).await?;
        let result = sqlx::query("PRAGMA user_version")
            .fetch_one(&connection)
            .await?;

        let version: u32 = result.get::<u32, usize>(0);
        if version != VERSION {
            bail!("database version {} is not supported {}", version, DATABASE);
        }
        Ok(connection)
    }

    async fn setup_new(db: &str) -> anyhow::Result<SqlitePool> {
        let connection = SqlitePool::connect(db).await?;
        let sql = format!(
            r"
            PRAGMA user_version = {};
            CREATE TABLE IF NOT EXISTS keyvalue (key TEXT PRIMARY KEY, value blob);
            ",
            VERSION
        );

        Ok(sqlx::query(&sql)
            .execute(&connection)
            .await
            .map(|_| connection)?)
    }

    /// get value by key
    pub async fn get<T>(&self, key: &str) -> anyhow::Result<T>
    where
        for<'a> T: Send + serde::Deserialize<'a>,
    {
        let result = sqlx::query("SELECT value FROM keyvalue WHERE key = ?")
            .bind(key)
            .fetch_one(&self.connection)
            .await?
            .get::<Vec<u8>, &str>("value");
        bincode::deserialize(&result).context("deserialize error")
    }

    /// Set a value given a key. a value can be anything that can be encoded to a sqlite object.
    /// i.e. implements the [sqlx::Encode] trait.
    pub async fn set<T>(&self, key: &str, value: T) -> anyhow::Result<()>
    where
        T: Send + serde::Serialize,
    {
        let encoded = bincode::serialize(&value)?;
        sqlx::query("INSERT OR REPLACE INTO keyvalue (key, value) VALUES (?,?)")
            .bind(key)
            .bind(encoded)
            .execute(&self.connection)
            .await
            .map(|_| ())
            .context("sqlite error")
    }
}
