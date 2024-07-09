use tokio::net::windows::named_pipe::ClientOptions;

#[tokio::main]
async fn main() {
    println!("Connecting to server...");

    let client = loop {
        tokio::time::sleep(tokio::time::Duration::from_millis(300)).await;

        let options = ClientOptions::new().open(r"\\.\pipe\StudioDumper");

        match options {
            Ok(client) => break client,
            Err(_) => {
                println!("An error occured. Retrying...");
                continue;
            },
        }
    };

    println!("Connected!");

    loop {
        client.readable().await.unwrap();

        let mut data = Vec::new();

        let size = match client.try_read(&mut data) {
            Ok(n) => n,
            Err(_) => { 
                continue;
            }
        };

        println!("Message of {} bytes: {:?}", size, data)
    }
}
