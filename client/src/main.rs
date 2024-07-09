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

    let mut buffer = vec![0; 1024 * 512];

    loop {
        client.readable().await.unwrap();

        let size = match client.try_read(&mut buffer) {
            Ok(n) => if n == 0 { continue } else { n },
            Err(_) => continue,
        };

        println!("Incoming Packet: {:02X?}", &buffer[..size])
    }
}
