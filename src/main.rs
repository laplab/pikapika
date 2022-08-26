#[link(name = "pico")]
extern {
	fn hello_laplab();
}

fn main() {
    unsafe {
        hello_laplab();
    }
    println!("Hello, world!");
}
