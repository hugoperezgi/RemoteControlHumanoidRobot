package app;

import java.net.InetSocketAddress;
import java.net.ServerSocket;
import controllers.ClientHandler;
import controllers.FirebaseController;

public class Main {
    public static void main(String[] args) throws Exception {   
        ServerSocket s = new ServerSocket();
        InetSocketAddress serverIPAddr = new InetSocketAddress("127.0.0.1",50500); 
        s.bind(serverIPAddr); 
        while (true){
            try {
                ClientHandler clientThread = new ClientHandler(s.accept());
                clientThread.setDaemon(true);
                clientThread.start();
            } catch (Exception e) {
                System.err.println(".accept() Error");
                e.printStackTrace();
            }
        }
        // fb();
    }


    // private static void fb()throws Exception{
    //     FirebaseController f = new FirebaseController();
    //     f.ini();
    //     System.out.println(f.getCurrentPos(0));
    //     System.out.println(f.setCurrentPos(0,139));
    //     System.out.println(f.getCurrentPos(0));
    //     System.out.println(f.setCurrentPos(0,178));
    //     System.out.println(f.setCurrentPos(0,187));
    //     System.out.println(f.getCurrentPos(0));
    //     System.out.println(f.setCurrentPos(0,101));
    //     System.out.println(f.getCurrentPos(0));
    //     System.out.println(f.setCurrentPos(0,27));
    //     System.out.println(f.getCurrentPos(0));
    //     f.setUpdatePostion();
    //     f.close();
    // }
}
