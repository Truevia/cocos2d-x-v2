package org.ccpkg;

import android.content.Intent;
import android.os.Bundle;

import org.ccpkg.Bridge;
// import org.ccpkg.Logger;
import org.cocos2dx.lib.Cocos2dxActivity;

public class PluginBaseActivity extends Cocos2dxActivity {

    static final String TAG = "PluginBaseActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Workaround in https://stackoverflow.com/questions/16283079/re-launch-of-activity-on-home-button-but-only-the-first-time/16447508
        if (!isTaskRoot()) {
            // Android launched another instance of the root activity into an existing task
            //  so just quietly finish and go away, dropping the user back into the activity
            //  at the top of the stack (ie: the last state of this task)
            return;
        }

        //
        try {
            Bridge.init(this);
            PluginBase.init(this);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        PluginBase.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        PluginBase.onStop();
    }

    @Override
    protected void onResume() {
        super.onResume();
        PluginBase.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        PluginBase.onPause();
    }

    @Override
    public void onBackPressed() {
        if (!PluginBase.onBackPressed()) {
            super.onBackPressed();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (!PluginBase.onActivityResult(requestCode, resultCode, data)) {
            super.onActivityResult(requestCode, resultCode, data);
        }
    }
}
