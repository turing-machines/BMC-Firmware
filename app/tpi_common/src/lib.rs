pub mod c_interface;
pub mod power;

#[repr(C)]
pub enum NodeId {
    Node1,
    Node2,
    Node3,
    Node4,
    All,
}
