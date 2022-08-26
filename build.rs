fn main() {
    println!("cargo:rustc-flags=-L cpp/bin");
    println!("cargo:rustc-flags=-l dylib=c++");
}