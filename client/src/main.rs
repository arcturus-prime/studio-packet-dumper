mod packet;

use tokio::{io::AsyncReadExt, net::windows::named_pipe::ClientOptions};

#[tokio::main(flavor = "current_thread")]
async fn main() {
    println!("Connecting to server...");

    let mut client = loop {
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

        let Ok(size) = client.read_u32_le().await else {
            continue;
        };

        if let Err(_) = client.read_exact(&mut buffer[..size as usize]).await {
            continue;
        }

        println!("Incoming Packet: {:02X?}", &buffer[..size as usize]);
    }
}
