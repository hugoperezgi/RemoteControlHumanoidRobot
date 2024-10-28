package app;

import java.net.InetSocketAddress;
import java.net.ServerSocket;
import controllers.ClientHandler;
import controllers.FirebaseController;

public class Main {

    public static boolean shutdown=false;

    public static void main(String[] args) throws Exception {   
        Main.shutdown=false;
        ServerSocket s = new ServerSocket();
        InetSocketAddress serverIPAddr = new InetSocketAddress("127.0.0.1",50500); 
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
        // fb();
    }


    // private static void fb()throws Exception{
    //     FirebaseController f = new FirebaseController();
    //     f.ini();
    //     f.setServoUpdateFlag(0b1<<28);
    //     System.out.println(f.getServoUpdateFlag());
    //     System.out.println(0b1<<28);
    //     f.close();
    // }
}
