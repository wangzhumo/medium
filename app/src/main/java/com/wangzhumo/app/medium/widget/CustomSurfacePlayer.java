package com.wangzhumo.app.medium.widget;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

/**
 * player container
 */
public class CustomSurfacePlayer implements SurfaceHolder.Callback {


    /*
     * load native code
     */
    static {
        System.loadLibrary("medium");
    }


    /*
     * local fields
     */
    private SurfaceHolder surfaceHolder;




    /*
     * set surfaceView and callback
     */
    public void setSurfaceView(SurfaceView surfaceView){
        // already has ?
        if (surfaceHolder != null){
            surfaceHolder.removeCallback(this);
        }
        this.surfaceHolder = surfaceView.getHolder();
        this.surfaceHolder.addCallback(this);
    }

    public void setDataSource(String filePath){

    }

    public native void start(String filePath, Surface surface);


    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        this.surfaceHolder = surfaceHolder;
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        this.surfaceHolder = surfaceHolder;
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {

    }
}
