package com.wangzhumo.app.medium;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import com.wangzhumo.app.medium.databinding.ActivityMainBinding;
import com.wangzhumo.app.medium.widget.CustomSurfacePlayer;


public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding mainBinding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // create customSurfacePlayer
        CustomSurfacePlayer surfacePlayer = new CustomSurfacePlayer();
        // set current surfaceView
        surfacePlayer.setSurfaceView(mainBinding.surfaceView);
    }

}
