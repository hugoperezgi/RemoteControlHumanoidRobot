package app;

import java.net.InetSocketAddress;
import java.net.ServerSocket;
import controllers.ClientHandler;

public class Main {

    public static boolean shutdown=false;

    public static void main(String[] args) throws Exception {   
        Main.shutdown=false;
        ServerSocket s = new ServerSocket();
        // InetSocketAddress serverIPAddr = new InetSocketAddress("10.60.122.206",57573); 
        InetSocketAddress serverIPAddr = new InetSocketAddress("192.168.1.164",57573); 
        s.bind(serverIPAddr); 
        while (!Main.shutdown){
            try {
                ClientHandler clientThread = new ClientHandler(s.accept());
                clientThread.setDaemon(true);
                clientThread.start();
            } catch (Exception e) {
                System.err.println(".accept() Error");
                e.printStackTrace();
            }
        }
        ClientHandler.fb.close();
        s.close();
    }

}
