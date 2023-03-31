pub fn add(x: i32, y: i32) -> i32 {
    x + y
}

#[no_mangle]
pub extern "C" fn c_add(x: i32, y: i32) -> i32 {
    add(x, y)
}
