mod packet;

use tokio::net::windows::named_pipe::ClientOptions;

#[tokio::main(flavor = "current_thread")]
async fn main() {
    println!("Connecting to server...");

    let client = loop {
        let options = ClientOptions::new().open(r"//./pipe/StudioDumper");

        match options {
            Ok(client) => break client,
            Err(_) => continue,
        }
    };

    println!("Connected!");

    loop {
        client.readable().await.unwrap();

        let mut data = Vec::with_capacity(4096);

        match client.try_read(&mut data) {
            Ok(n) => println!("Received message of length {}: {:?}", n, data),
            Err(_) => continue,
        }
    }
}
