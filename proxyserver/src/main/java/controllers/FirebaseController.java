package controllers;

import java.io.FileInputStream;
import java.io.IOException;
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
    private static Map readMap, tmpMap;
    private static boolean waitWriting;
    
    public FirebaseController(){
        firebaseApp=null;
        database=null;
        tpRef=null;
        readMap=null;
        waitWriting=true;
    }

    public void ini() throws IOException{
        if(firebaseApp!=null){if(database==null){database = FirebaseDatabase.getInstance().getReference();}else{return;}}
        Scanner scn = new Scanner(new File("src\\main\\resources\\controllers\\url.txt"));
        String url = scn.nextLine();
        System.out.println(url);
        FileInputStream serviceAccount=new FileInputStream("src\\main\\resources\\controllers\\serviceAccountKey.json");
        FirebaseOptions options = new FirebaseOptions.Builder()
        .setCredentials(GoogleCredentials.fromStream(serviceAccount))
        .setDatabaseUrl(url)
        .build();
        firebaseApp = FirebaseApp.initializeApp(options);
        database = FirebaseDatabase.getInstance().getReference();
    }

    public int setTargetPos(int srvId, int servoAngle) throws InterruptedException{
        waitWriting=true;
        if(srvId<0||srvId>20){return -1;}
        if(servoAngle<0||servoAngle>180){return 1;}
        tpRef=database.child("targetPosition");
        Map<String, Object> tpUpdt=new HashMap<>();
        tpUpdt.put("servo"+String.valueOf(srvId),servoAngle);
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
            if(i<0||i>20){return -1;}
            if(servoAngle[j]<0||servoAngle[j]>180){return 1;}
            tpUpdt.put("servo"+String.valueOf(i),servoAngle[j]);
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
        if(srvId<0||srvId>20){return -1;}
        if(servoAngle<0||servoAngle>180){return 1;}
        
        tpRef=database.child("currentPosition");
        Map<String, Object> tpUpdt=new HashMap<>();
        tpUpdt.put("servo"+String.valueOf(srvId),servoAngle);
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

    public Map getALLCurrentPos() throws InterruptedException{
        
        tpRef=database.child("currentPosition");
        tpRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                tmpMap=(Map<String, Object>) dataSnapshot.getValue();
            }
          
            @Override
            public void onCancelled(DatabaseError databaseError) {}
        });
        do {
            Thread.sleep(100);
        } while (tmpMap==null);
        System.out.println(readMap.toString());
        readMap=tmpMap;tmpMap=null;
        return readMap;
    }
    public long getCurrentPos(int srvId) throws InterruptedException{
        if(srvId<0||srvId>20){return -1;}
        tpRef=database.child("currentPosition");
        tmpMap=null;
        tpRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                tmpMap=(Map<String, Object>) dataSnapshot.getValue();
            }
          
            @Override
            public void onCancelled(DatabaseError databaseError) {}
        });
        do {
            Thread.sleep(100);
        } while (tmpMap==null);
        readMap=tmpMap;tmpMap=null;
        return (long) readMap.get("servo"+String.valueOf(srvId));
    }

    public Map getALLTargetPos() throws InterruptedException{
        
        tpRef=database.child("targetPosition");
        tpRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                tmpMap=(Map<String, Object>) dataSnapshot.getValue();
            }
          
            @Override
            public void onCancelled(DatabaseError databaseError) {}
        });
        do {
            Thread.sleep(100);
        } while (tmpMap==null);
        System.out.println(readMap.toString());
        readMap=tmpMap;tmpMap=null;
        return readMap;
    }
    public long getTargetPos(int srvId) throws InterruptedException{
        if(srvId<0||srvId>20){return -1;}
        tpRef=database.child("targetPosition");
        tmpMap=null;
        tpRef.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                tmpMap=(Map<String, Object>) dataSnapshot.getValue();
            }
          
            @Override
            public void onCancelled(DatabaseError databaseError) {}
        });
        do {
            Thread.sleep(100);
        } while (tmpMap==null);
        readMap=tmpMap;tmpMap=null;
        return (long) readMap.get("servo"+String.valueOf(srvId));
    }



    public void close(){
        firebaseApp.delete();
    }
}
