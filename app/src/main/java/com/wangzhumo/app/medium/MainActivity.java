package com.wangzhumo.app.medium;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;

import com.bumptech.glide.Glide;
import com.wangzhumo.app.medium.databinding.ActivityMainBinding;
import com.wangzhumo.app.medium.utils.GlideLoadEngine;
import com.wangzhumo.app.medium.widget.CustomSurfacePlayer;
import com.zhihu.matisse.Matisse;
import com.zhihu.matisse.MimeType;
import com.zhihu.matisse.internal.entity.CaptureStrategy;

import java.io.File;
import java.util.List;



public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding mainBinding;
    private CustomSurfacePlayer surfacePlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mainBinding = ActivityMainBinding.inflate(this.getLayoutInflater());
        setContentView(R.layout.activity_main);

        // create customSurfacePlayer
        surfacePlayer = new CustomSurfacePlayer();
        // set current surfaceView
        surfacePlayer.setSurfaceView(mainBinding.surfaceView);
//        mainBinding.btVideoStart.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                // 开启图片选择
//                selectPic();
//                Log.e("MainActivity ","1 selectPic");
//            }
//        });
        findViewById(R.id.bt_video_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.e("MainActivity ","1 selectPic");
                File video = new File(Environment.getExternalStorageDirectory(), "overwatch_echo.mp4");
                Log.e("MainActivity ","2 path : " + video.getAbsolutePath());
                surfacePlayer.setDataSource(video.getAbsolutePath());
            }
        });
    }

    void selectPic()
    {
        Matisse.from(this)
                .choose(MimeType.ofVideo())
                .captureStrategy(new CaptureStrategy(true, "com.wangzhumo.app.medium.picker"))
                .theme(R.style.Matisse_Dracula)
                .countable(true)
                .maxSelectable(1)
                .restrictOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT)
                .thumbnailScale(0.87f)
                .imageEngine(new GlideLoadEngine())
                .forResult(REQUEST_CODE_CHOOSE_PHOTO_ALBUM);
    }



    private final int REQUEST_CODE_CHOOSE_PHOTO_ALBUM = 1;


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_CODE_CHOOSE_PHOTO_ALBUM && resultCode == RESULT_OK) {
            //图片路径 同样视频地址也是这个 根据requestCode
            List<String> pathList = Matisse.obtainPathResult(data);

            surfacePlayer.setDataSource(pathList.get(0));
        }
    }
}
