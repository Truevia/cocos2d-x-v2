// package org.ccpkg;

// import android.app.Activity;
// import android.content.Context;
// import android.content.Intent;
// import android.util.Log;
// import android.view.View;

// import com.google.android.gms.ads.AdError;
// import com.google.android.gms.ads.AdListener;
// import com.google.android.gms.ads.AdRequest;
// import com.google.android.gms.ads.AdSize;
// import com.google.android.gms.ads.AdView;
// import com.google.android.gms.ads.FullScreenContentCallback;
// import com.google.android.gms.ads.LoadAdError;
// import com.google.android.gms.ads.MobileAds;
// import com.google.android.gms.ads.initialization.InitializationStatus;
// import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
// import com.google.android.gms.ads.interstitial.InterstitialAd;
// import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;

// import org.ccpkg.GameUtil;
// import org.ccpkg.Logger;
// import org.ccpkg.PluginBase;
// import org.ccpkg.PluginIAP;
// import org.ccpkg.PluginListener;

// import java.util.HashMap;
// import java.util.Map;

// public class PluginAdMob implements PluginListener {
//     private static String TAG = "admob";
//     private Context _context = null;

//     private Map<String, AdView> _bannerMap = new HashMap<>();
//     private InterstitialAd _interstitialAd;

//     public PluginAdMob(Context context) {
//         _context = context;
//         PluginBase.addListener(this);
//         Log.e(TAG, "Plugin create");
//     }

//     public void init(String string) {
//         Log.e("rr", "PluginAdmob:init");
//         MobileAds.initialize(getActivity(), new OnInitializationCompleteListener() {
//             @Override
//             public void onInitializationComplete(InitializationStatus initializationStatus) {
//                 Log.d(TAG, "MobileAds onInitializationComplete");
//             }
//         });

//         createBanner("top", "ca-app-pub-3940256099942544/6300978111", "bottom");
//         createInterstitialAd("");
//     }

//     public String getVersion(String data) {
//         return "1.2.3";
//     }

//     public void show(String name) {
//         try {
//             if (_bannerMap.containsKey(name)) {
//                 AdView adView = _bannerMap.get(name);
//                 adView.setVisibility(View.VISIBLE);
//                 return;
//             }
//             if (_interstitialAd != null) {
//                 _interstitialAd.show(getActivity());
//                 return;
//             }
//         } catch (Exception e) {
//             e.printStackTrace();
//         }
//     }

//     @Override
//     public void onStart() {

//     }

//     @Override
//     public void onStop() {

//     }

//     @Override
//     public void onResume() {

//     }

//     @Override
//     public void onPause() {

//     }

//     @Override
//     public void onDestroy() {

//     }

//     @Override
//     public boolean onBackPressed() {
//         return false;
//     }

//     @Override
//     public boolean onActivityResult(int i, int i2, Intent intent) {
//         return false;
//     }

//     // testId: "ca-app-pub-3940256099942544/6300978111"
//     private AdView createBanner(String name, String unitId, String alignment) {

//         AdView adView = new AdView(getActivity());
//         adView.setAdSize(AdSize.SMART_BANNER);
//         adView.setAdUnitId(unitId);
//         AdRequest adRequest = new AdRequest.Builder().build();
//         adView.loadAd(adRequest);
//         adView.setAdListener(new AdListener() {
//             @Override
//             public void onAdClicked() {
//                 // Code to be executed when the user clicks on an ad.
//                 Log.e(TAG, "onAdClicked");
//             }

//             @Override
//             public void onAdClosed() {
//                 // Code to be executed when the user is about to return
//                 // to the app after tapping on an ad.
//                 Log.e(TAG, "onAdClosed");
//             }

//             @Override
//             public void onAdFailedToLoad(LoadAdError adError) {
//                 // Code to be executed when an ad request fails.
//                 Log.e(TAG, "onAdFailedToLoad");
//             }

//             @Override
//             public void onAdImpression() {
//                 // Code to be executed when an impression is recorded
//                 // for an ad.
//                 Log.e(TAG, "onAdImpression");
//             }

//             @Override
//             public void onAdLoaded() {
//                 // Code to be executed when an ad finishes loading.
//                 Log.e(TAG, "onAdLoaded");

//                 Bridge.emit("PluginAdMob", "onAdLoaded", "");

//                 GameUtil.addToRootView(getActivity(), adView, alignment);
//                 adView.setVisibility(View.VISIBLE);

//                 throw new RuntimeException("Test Crash"); // Force a crash
//             }

//             @Override
//             public void onAdOpened() {
//                 // Code to be executed when an ad opens an overlay that
//                 // covers the screen.
//                 Log.e(TAG, "onAdOpened");
//             }
//         });

//         adView.setVisibility(View.GONE);
//         return adView;
//     }

//     public void createInterstitialAd(String name) {
//         AdRequest adRequest = new AdRequest.Builder().build();

//         InterstitialAd.load(_context,"ca-app-pub-3940256099942544/1033173712", adRequest,
//             new InterstitialAdLoadCallback() {
//                 @Override
//                 public void onAdLoaded(InterstitialAd interstitialAd) {
//                     // The mInterstitialAd reference will be null until
//                     // an ad is loaded.
//                     _interstitialAd = interstitialAd;
//                     Log.i(TAG, "onAdLoaded");
//                 }

//                 @Override
//                 public void onAdFailedToLoad(LoadAdError loadAdError) {
//                     // Handle the error
//                     Log.d(TAG, loadAdError.toString());
//                     _interstitialAd = null;
//                 }
//             });
//     }

//     public void showBanner() {

//     }
//     public void showInterstitialAd() {
//         if (_interstitialAd == null) {
//             return;
//         }

//         _interstitialAd.setFullScreenContentCallback(new FullScreenContentCallback(){
//             @Override
//             public void onAdClicked() {
//                 // Called when a click is recorded for an ad.
//                 Log.d(TAG, "Ad was clicked.");
//             }

//             @Override
//             public void onAdDismissedFullScreenContent() {
//                 // Called when ad is dismissed.
//                 // Set the ad reference to null so you don't show the ad a second time.
//                 Log.d(TAG, "Ad dismissed fullscreen content.");
//                 _interstitialAd = null;
//             }

//             @Override
//             public void onAdFailedToShowFullScreenContent(AdError adError) {
//                 // Called when ad fails to show.
//                 Log.e(TAG, "Ad failed to show fullscreen content.");
//                 _interstitialAd = null;
//             }

//             @Override
//             public void onAdImpression() {
//                 // Called when an impression is recorded for an ad.
//                 Log.d(TAG, "Ad recorded an impression.");
//             }

//             @Override
//             public void onAdShowedFullScreenContent() {
//                 // Called when ad is shown.
//                 Log.d(TAG, "Ad showed fullscreen content.");
//             }
//         });
//         _interstitialAd.show(getActivity());
//     }

//     private Activity getActivity() {
//         return (Activity) _context;
//     }
// }
