package com.wangzhumo.app.medium.widget;

import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.File;

import androidx.annotation.NonNull;

/**
 * player container
 */
public class CustomSurfacePlayer implements SurfaceHolder.Callback {


    /*
     * load native code
     */
    static {
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("swscale");
        System.loadLibrary("avfilter");
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
        start(filePath,surfaceHolder.getSurface());
    }

    public native void start(String filePath, Surface surface);


    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        this.surfaceHolder = surfaceHolder;
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {

    }

    public SurfaceHolder getSurfaceHolder() {
        return surfaceHolder;
    }
}
