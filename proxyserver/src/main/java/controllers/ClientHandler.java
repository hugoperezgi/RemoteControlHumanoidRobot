package controllers;

import java.io.IOException;
import java.net.Socket;

import app.Main;

public class ClientHandler extends Thread{
    
    boolean run = true;
    private Socket sck;
    byte[] byteBuff = new byte[255];

    static boolean updateModeRT=false;
    public static FirebaseController fb=null;

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
                /* maybe change for something like !s-SRVP-<Number of servos to update>-<servoid>:<position>-e! -> [!s][SRVP][number of servos to update][servoid:servopos~servoid:servopos][e!] */
                // q = "!s-SRVP-";q+=id;q+="-";q+=pos;q+="-e!" [!s][SRVP][id][pos][e!]
                //  eMOD - updateModeRT(RT/delayed), param1 expected (0-Delayed, 1-RealTime)
                // q = "!s-eMOD-";q+=mode;q+="-e!"; [!s][SRVP][0/1][e!]
                //  sOFF - Server off
                // q = "!s-eMOD-";q+=mode;q+="-e!"; [!s][SRVP][0/1][e!]


                if(!(s.startsWith("!s-")&&s.endsWith("-e!"))){
                    //TODO Invalid query format
                    System.out.println("ERROR_INVALIDFORMAT");
                }


                //Switch query
                switch(s.split("-")[1]){
                    case "mALL":
                        if(updateModeRT){/* TODO return ERROR_INVALIDOP */System.out.println("ERROR_INVALIDOP");}
                        else{
                            try {
                                fb.setUpdatePostion();
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        return;
                    case "SRVP":
                        String AKCFlag=updateServoPostitions(Integer.parseInt(s.split("-")[2]),s.split("-")[3]);
                        System.out.println(AKCFlag);
                        /* TODO ACK back to cli */
                        return;
                    case "eMOD":
                        if(s.split("-")[2].equals("0")){
                            updateModeRT=false;
                            /* ACK back to cli */
                        }else if(s.split("-")[2].equals("1")){
                            updateModeRT=true;
                            /* ACK back to cli */
                        }
                        return;
                    case "sOFF": 
                        Main.shutdown=true;
                        try{Thread.sleep(5000);}catch(Exception e){};
                        fb.close();
                        System.exit(0);
                        return;
                    default: /* TODO return ERROR_INVALIDFORMAT */System.out.println("ERROR_INVALIDFORMAT");
                        return;
                }

                
            } catch (IOException e) {
                run=false;
            }
        }
        
    }

    private String updateServoPostitions(int servoCount, String servoPositions){

        try {
            fb.setNewInfo();
        } catch (InterruptedException e) {
            e.printStackTrace();
            return "ERROR_FBCON";
        }
        String[] sPosA=servoPositions.split("~");
        if(sPosA.length!=servoCount){return "ERROR_INVALIDFORMAT";}
        int[] servoIds=new int[servoCount], servoPos=new int[servoCount];
        int servoFlag=0;
        try {
            servoFlag=fb.getServoUpdateFlag();
        } catch (InterruptedException e) {
            e.printStackTrace();
            return "ERROR_FBCON";
        }
        int i=0;
        for (String s : sPosA) {
            servoIds[i]=Integer.parseInt(s.split(":")[0]);
            servoFlag|=0b1<<servoIds[i];
            servoPos[i]=Integer.parseInt(s.split(":")[1]);
            i++;
        }
        try {
            fb.setServoUpdateFlag(servoFlag);
            fb.setMultipleTargetPos(servoIds,servoPos);
        } catch (InterruptedException e) {
            e.printStackTrace();
            return "ERROR_FBCON";
        }
        if(updateModeRT){
            try {
                fb.setUpdatePostion();
            } catch (InterruptedException e) {
                e.printStackTrace();
                return "ERROR_FBCON";
            }
    
        }
        return String.valueOf(servoFlag);
    }

    public ClientHandler(Socket s) throws IOException{
        this.sck=s;
        if(fb==null){fb=new FirebaseController(); fb.ini();}
    }

}
