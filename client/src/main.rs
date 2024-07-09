use tokio::{io::Interest, net::windows::named_pipe::ClientOptions};

#[tokio(main)]
async fn main() {
   println!("Starting server...");

    let client = loop {
        let options = ClientOptions::new().open(r"//./pipe/StudioDumper");

        match options {
            Ok(client) => break client,
            Err(e) if e.raw_os_error() == Some(ERROR_PIPE_BUSY.0 as i32) => println!("Pipe busy! Retrying..."),
            Err(_) => println!("An unknown error occured. Retrying..."),
        }
    };

    let ready = client.ready(Interest::READABLE).await.unwrap();

    println!("Server started!");
}
