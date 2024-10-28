package controllers;

import java.io.IOException;
import java.net.Socket;

import app.Main;

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

                s=s.trim();

                /* TODO */
                //Check query format (Only Firebase ones tho)
                //  mALL - moveAll servos - AKA execute movement update
                // q = "!s-mALL-e!" [!s][mALL][e!]
                //  SRVP - changeServoPosition, param1 (servoID), param2 (servoPosition)
                /* maybe change for something like !s-SRVP-<Number of servos to update>-<servoid>:<position>-e! -> [!s][SRVP][number of servos to update][servoid:servopos][e!] */
                // q = "!s-SRVP-";q+=id;q+="-";q+=pos;q+="-e!" [!s][SRVP][id][pos][e!]
                //  eMOD - updateModeRT(RT/delayed), param1 expected (0-Delayed, 1-RealTime)
                // q = "!s-eMOD-";q+=mode;q+="-e!"; [!s][SRVP][0/1][e!]
                //  sOFF - Server off
                // q = "!s-eMOD-";q+=mode;q+="-e!"; [!s][SRVP][0/1][e!]


                if(!(s.startsWith("!s-")&&s.endsWith("-e!"))){
                    //TODO Invalid query format
                }


                //Switch query
                switch(s.split("-")[1]){
                    case "mALL"://TODO move all servos
                        return;
                    case "SRVP"://TODO update servo position and update control code!!!
                        return;
                    case "eMOD"://TODO update mode
                        return;
                    case "sOFF": 
                        Main.shutdown=true;
                        try{Thread.sleep(5000);}catch(Exception e){};
                        System.exit(0);
                        return;
                    default: //TODO Invalid query format
                        return;
                }

                
            } catch (IOException e) {
                run=false;
            }
        }
        
    }

    public ClientHandler(Socket s){
        this.sck=s;
    }

}
