use anyhow::bail;
use anyhow::Context;

use sqlx::Row;
use sqlx::Sqlite;
use sqlx::SqlitePool;
use std::path::PathBuf;

/// directory on target which the sqlite database is written to.
const DATABASE: &str = concat!("/var/lib/", env!("CARGO_PKG_NAME"), "/bmc.db");

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
        let sql_db = format!("sqlite:{}", DATABASE);

        let connection = SqlitePool::connect(&sql_db).await?;

        if !path.exists() {
            if let Some(p) = path.parent() {
                tokio::fs::create_dir_all(p).await?
            }
            Self::setup_new(&connection).await?;
        } else {
            Self::verify_version(&connection).await?;
        }

        Ok(Self { connection })
    }

    async fn verify_version(connection: &SqlitePool) -> anyhow::Result<()> {
        let result = sqlx::query("PRAGMA user_version")
            .fetch_one(connection)
            .await?;

        let version: u32 = result.get::<u32, usize>(0);
        if version != 1 {
            bail!("database version {} is not supported {}", version, DATABASE);
        }
        Ok(())
    }

    async fn setup_new(connection: &SqlitePool) -> std::result::Result<(), sqlx::Error> {
        let sql = r"
            PRAGMA user_version = 1; 
            CREATE TABLE IF NOT EXISTS keyvalue (key TEXT PRIMARY KEY, value blob);
            ";

        Ok(sqlx::query(sql).execute(connection).await.map(|_| ())?)
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

    //    /// Set a value represented as a bitfield.
    //    ///
    //    /// # Arguments
    //    ///
    //    /// * 'bits'    value of the key/value pair to write.
    //    /// * 'mask'    indicates which values need to be updated to either 'on' or 'off'. e.g. 0b1001
    //    /// means that the value of the the 1st and the 4th bit will be written to the value of the
    //    /// corresponding bit in the 'bits' argument.
    //    pub async fn set_with_bitmask(&self, key: &str, bits: u32, mask: u32) -> anyhow::Result<u32> {
    //        let current: u32 = self.get(key).await?;
    //        let mut result = current;
    //        for n in 0..size_of::<u32>() {
    //            // continue if bit is not to be written
    //            if (mask & (1 << n)) == 0 {
    //                continue;
    //            }
    //
    //            // check if bit need to written high or low
    //            let high = (bits & (1 << n)) != 0;
    //
    //            result = if high {
    //                result | (1 << n)
    //            } else {
    //                result & !(1 << n)
    //            };
    //        }
    //
    //        if current != result {
    //            self.set(key, result).await?;
    //        }
    //        Ok(result)
    //    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_wrong_version_check() {
        let connection = SqlitePool::connect("sqlite::memory:").await.unwrap();
        let sql = r"
            PRAGMA user_version = 3; 
            ";

        sqlx::query(sql).execute(&connection).await.unwrap();
        let result = ApplicationPersistency::verify_version(&connection).await;
        assert!(result.is_err());
    }

    macro_rules! test_bits {
        ($db: ident, $(($bits:literal,$mask:literal,$output:literal)),+ ) => {
            $(  $db.set_with_bitmask("foo", $bits, $mask).await.unwrap();
                assert_eq!($output, $db.get::<u32>("foo").await.unwrap());
            )*
        };
    }

    #[tokio::test]
    async fn test_power_bits() {
        let connection = SqlitePool::connect("sqlite::memory:").await.unwrap();
        ApplicationPersistency::setup_new(&connection)
            .await
            .unwrap();
        ApplicationPersistency::verify_version(&connection)
            .await
            .unwrap();

        let db = ApplicationPersistency { connection };
        db.set("foo", 0).await.unwrap();

        test_bits!(
            db,
            (0b1111, 0b0000, 0b0000),
            (0b1111, 0b1000, 0b1000),
            (0b1111, 0b0000, 0b1000),
            (0b0011, 0b1111, 0b0011),
            (0b0000, 0b1111, 0b0000)
        );
    }
}
