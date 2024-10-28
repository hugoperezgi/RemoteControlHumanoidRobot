package controllers;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.io.File;

import com.google.auth.oauth2.GoogleCredentials;
import com.google.firebase.FirebaseApp;
import com.google.firebase.FirebaseOptions;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class FirebaseController {

    private static FirebaseApp firebaseApp;
    private static DatabaseReference database;
    private static DatabaseReference tpRef;
    private static ArrayList readList, tmpList;
    private static boolean waitWriting;
	private static Integer tmpInt;
    
    public FirebaseController(){
        firebaseApp=null;
        database=null;
        tpRef=null;
        readList=null;
		tmpInt=null;
        waitWriting=true;
    }

    public void ini() throws IOException{
        if(firebaseApp!=null){if(database==null){database = FirebaseDatabase.getInstance().getReference();}else{return;}}
        Scanner scn = new Scanner(new File("src\\main\\resources\\controllers\\url.txt"));
        String url = scn.nextLine();
        FileInputStream serviceAccount=new FileInputStream("src\\main\\resources\\controllers\\serviceAccountKey.json");
        FirebaseOptions options = new FirebaseOptions.Builder()
        .setCredentials(GoogleCredentials.fromStream(serviceAccount))
        .setDatabaseUrl(url)
        .build();
        firebaseApp = FirebaseApp.initializeApp(options);
        database = FirebaseDatabase.getInstance().getReference();
        scn.close();
    }

    public int setTargetPos(int srvId, int servoAngle) throws InterruptedException{
        waitWriting=true;
        if(srvId<0||srvId>26){return -1;}
        if(servoAngle<0||servoAngle>180){return 1;}
        tpRef=database.child("targetPosition");
        Map<String, Object> tpUpdt=new HashMap<>();
        tpUpdt.put(String.valueOf(srvId),servoAngle);
        tpRef.updateChildren(tpUpdt, new DatabaseReference.CompletionListener() {
            @Override
            public void onComplete(DatabaseError databaseError, DatabaseReference databaseReference) {
              if (databaseError != null) {
                System.out.println("Data could not be saved " + databaseError.getMessage());
              } 
              waitWriting=false;
            }
          });
        do {            
            Thread.sleep(100);
        } while (waitWriting);waitWriting=true;
        return 0;
    }

    public int setMultipleTargetPos(int[] srvId, int[] servoAngle) throws InterruptedException{
        waitWriting=true;
        tpRef=database.child("targetPosition");
        Map<String, Object> tpUpdt=new HashMap<>();
        short j=0;
        for (int i : srvId) {
            if(i<0||i>26){return -1;}
            if(servoAngle[j]<0||servoAngle[j]>180){return 1;}
            tpUpdt.put(String.valueOf(i),servoAngle[j]);
            j++;
        }
        
        tpRef.updateChildren(tpUpdt, new DatabaseReference.CompletionListener() {
            @Override
            public void onComplete(DatabaseError databaseError, DatabaseReference databaseReference) {
              if (databaseError != null) {
                System.out.println("Data could not be saved " + databaseError.getMessage());
              } 
              waitWriting=false;
            }
          });
        do {            
            Thread.sleep(100);
        } while (waitWriting);waitWriting=true;
        return 0;
    }

    /**@Deprecated DO NOT USE THIS, THE MCU SHOULD BE THE ONE UPDATING THE CURRENT POSITIONS. Test only */ 
    public int setCurrentPos(int srvId, int servoAngle) throws InterruptedException{
        waitWriting=true;
        if(srvId<0||srvId>26){return -1;}
        if(servoAngle<0||servoAngle>180){return 1;}
        
        tpRef=database.child("currentPosition");
        Map<String, Object> tpUpdt=new HashMap<>();
        tpUpdt.put(String.valueOf(srvId),servoAngle);
        tpRef.updateChildrenAsync(tpUpdt);
        tpRef.updateChildren(tpUpdt, new DatabaseReference.CompletionListener() {
            @Override
            public void onComplete(DatabaseError databaseError, DatabaseReference databaseReference) {
              if (databaseError != null) {
                System.out.println("Data could not be saved " + databaseError.getMessage());
              } 
              waitWriting=false;
            }
          });
        do {            
            Thread.sleep(100);
        } while (waitWriting);waitWriting=true;
        return 0;
    }

    public int setUpdatePostion() throws InterruptedException{
        waitWriting=true;

        tpRef=database.getRef();
        Map<String, Object> tpUpdt=new HashMap<>();
        tpUpdt.put("UpdatePostion",true);
        tpRef.updateChildrenAsync(tpUpdt);
        tpRef.updateChildren(tpUpdt, new DatabaseReference.CompletionListener() {
            @Override
            public void onComplete(DatabaseError databaseError, DatabaseReference databaseReference) {
              if (databaseError != null) {
                System.out.println("Data could not be saved " + databaseError.getMessage());
              } 
              waitWriting=false;
            }
          });
        do {            
            Thread.sleep(100);
        } while (waitWriting);waitWriting=true;
        return 0;
    }

    public int setNewInfo() throws InterruptedException{
        waitWriting=true;

        tpRef=database.getRef();
        Map<String, Object> tpUpdt=new HashMap<>();
        tpUpdt.put("NewInfo",true);
        tpRef.updateChildrenAsync(tpUpdt);
        tpRef.updateChildren(tpUpdt, new DatabaseReference.CompletionListener() {
            @Override
            public void onComplete(DatabaseError databaseError, DatabaseReference databaseReference) {
              if (databaseError != null) {
                System.out.println("Data could not be saved " + databaseError.getMessage());
              } 
              waitWriting=false;
            }
          });
        do {            
            Thread.sleep(100);
        } while (waitWriting);waitWriting=true;
        return 0;
    }

    public ArrayList getALLCurrentPos() throws InterruptedException{
        
        tpRef=database.child("currentPosition");
        tpRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                tmpList= (ArrayList) dataSnapshot.getValue();
            }
          
            @Override
            public void onCancelled(DatabaseError databaseError) {}
        });
        do {
            Thread.sleep(100);
        } while (tmpList==null);
        readList=tmpList;tmpList=null;
        return readList;
    }
    public long getCurrentPos(int srvId) throws InterruptedException{
        if(srvId<0||srvId>26){return -1;}
        return (long) getALLCurrentPos().get(srvId);
    }

    public ArrayList getALLTargetPos() throws InterruptedException{
        
        tpRef=database.child("targetPosition");
        tpRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                tmpList=(ArrayList) dataSnapshot.getValue();
            }
          
            @Override
            public void onCancelled(DatabaseError databaseError) {}
        });
        do {
            Thread.sleep(100);
        } while (tmpList==null);
        readList=tmpList;tmpList=null;
        return readList;
    }

    public long getTargetPos(int srvId) throws InterruptedException{
        if(srvId<0||srvId>26){return -1;}
        return (long) getALLTargetPos().get(srvId);
    }

    public int setServoUpdateFlag(int servoControlFlag) throws InterruptedException{
      waitWriting=true;

      tpRef=database.getRef();
      Map<String, Object> tpUpdt=new HashMap<>();
      tpUpdt.put("ServoUpdateFlag",servoControlFlag);
      tpRef.updateChildrenAsync(tpUpdt);
      tpRef.updateChildren(tpUpdt, new DatabaseReference.CompletionListener() {
          @Override
          public void onComplete(DatabaseError databaseError, DatabaseReference databaseReference) {
            if (databaseError != null) {
              System.out.println("Data could not be saved " + databaseError.getMessage());
            } 
            waitWriting=false;
          }
        });
      do {            
          Thread.sleep(100);
      } while (waitWriting);waitWriting=true;
      return 0;
	}

    public int getServoUpdateFlag() throws InterruptedException{
		tpRef=database.child("ServoUpdateFlag");
		tmpInt=null;
        tpRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                tmpInt=((Long) dataSnapshot.getValue()).intValue();
            }
          
            @Override
            public void onCancelled(DatabaseError databaseError) {}
        });
        do {
            Thread.sleep(100);
        } while (tmpInt==null);
        return tmpInt.intValue();
	}

    public void close(){
        firebaseApp.delete();
    }
}
