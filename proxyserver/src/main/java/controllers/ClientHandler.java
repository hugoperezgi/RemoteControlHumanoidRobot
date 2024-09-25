package controllers;

import java.io.IOException;
import java.net.Socket;

public class ClientHandler extends Thread{
    
    boolean run = true;
    private Socket sck;
    byte[] byteBuff = new byte[255];

    @Override
    public void run() {
        run=true;

        while(run){
            try {
                
                //Get the query
                sck.getInputStream().read(byteBuff);
                String s="";
                for (byte b : byteBuff) {
                    if(b==0){break;}
                    s+=(char)b;
                }

                //Check query format
                /* TODO */

                //Switch query
                /* TODO */
                
            } catch (IOException e) {
                run=false;
            }
        }
        
    }

    public ClientHandler(Socket s){
        this.sck=s;
    }

}
