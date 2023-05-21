use anyhow::bail;
use anyhow::Context;

use log::info;
use sqlx::Row;
use sqlx::Sqlite;
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
    pub async fn get<T: Send + sqlx::Type<Sqlite> + for<'a> sqlx::Decode<'a, Sqlite>>(
        &self,
        key: &str,
    ) -> anyhow::Result<T> {
        let result = sqlx::query("SELECT value FROM keyvalue WHERE key = ?")
            .bind(key)
            .fetch_one(&self.connection)
            .await?;
        Ok(result.get::<T, &str>("value"))
    }

    /// Set a value given a key. a value can be anything that can be encoded to a sqlite object.
    /// i.e. implements the [sqlx::Encode] trait.
    pub async fn set<T>(&self, key: &str, value: T) -> anyhow::Result<()>
    where
        T: Send + sqlx::Type<Sqlite> + for<'a> sqlx::Encode<'a, Sqlite>,
    {
        sqlx::query("INSERT OR REPLACE INTO keyvalue (key, value) VALUES (?,?)")
            .bind(key)
            .bind(value)
            .execute(&self.connection)
            .await
            .map(|_| ())
            .context("sqlite error")
    }
}

//#[cfg(test)]
//mod tests {
//    use super::*;
//
//    #[tokio::test]
//    async fn test_wrong_version_check() {
//        let connection = SqlitePool::connect("sqlite::memory:").await.unwrap();
//        let sql = r"
//            PRAGMA user_version = 3;
//            ";
//
//        sqlx::query(sql).execute(&connection).await.unwrap();
//        let result = ApplicationPersistency::setup_new(&connection).await;
//        assert!(result.is_err());
//    }
//
//    macro_rules! test_bits {
//        ($db: ident, $(($bits:literal,$mask:literal,$output:literal)),+ ) => {
//            $(  $db.set_with_bitmask("foo", $bits, $mask).await.unwrap();
//                assert_eq!($output, $db.get::<u32>("foo").await.unwrap());
//            )*
//        };
//    }
//
//    // #[tokio::test]
//    // async fn test_power_bits() {
//    //     let connection = SqlitePool::connect("sqlite::memory:").await.unwrap();
//    //     ApplicationPersistency::setup_new(&connection)
//    //         .await
//    //         .unwrap();
//    //     ApplicationPersistency::verify_version(&connection)
//    //         .await
//    //         .unwrap();
//
//    //     let db = ApplicationPersistency { connection };
//    //     db.set("foo", 0).await.unwrap();
//
//    //     test_bits!(
//    //         db,
//    //         (0b1111, 0b0000, 0b0000),
//    //         (0b1111, 0b1000, 0b1000),
//    //         (0b1111, 0b0000, 0b1000),
//    //         (0b0011, 0b1111, 0b0011),
//    //         (0b0000, 0b1111, 0b0000)
//    //     );
//    // }
//}
