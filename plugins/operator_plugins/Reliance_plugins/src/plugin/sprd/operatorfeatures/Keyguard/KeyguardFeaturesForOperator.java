
package plugin.sprd.keyguardoperator;

import com.sprd.keyguard.KeyguardPluginsHelper;
import android.app.AddonManager;
import android.content.Context;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.util.Log;

public class KeyguardFeaturesForOperator extends KeyguardPluginsHelper implements
        AddonManager.InitialCallback {

    public static final String LOG_TAG = "KeyguardOperatorForReliance";
    private Context mAddonContext;

    public KeyguardFeaturesForOperator() {
    }

    @Override
    public Class onCreateAddon(Context context, Class clazz) {
        mAddonContext = context;
        return clazz;
    }

    /* SPRD: Bug 532196 modify for flight mode name display in reliance. @{ */
    public boolean showFlightMode() {
        Log.d(LOG_TAG, "ShowFlightMode = " + true);
        return true;
    }
    /* @} */

    /* SPRD: Bug#534387 modify for network name display in reliance. @{ */
    /**
     * Reliance case:
     * Can not show spn when plmn should be shown.
     */
    public String updateNetworkName(Context context, boolean showSpn, String spn, boolean showPlmn,
            String plmn) {
        Log.d(LOG_TAG, "updateNetworkName showSpn=" + showSpn + " spn=" + spn
                + " showPlmn=" + showPlmn + " plmn=" + plmn);
        StringBuilder str = new StringBuilder();
        if (showPlmn && plmn != null) {
            if (context.getString(com.android.internal.R.string.emergency_calls_only)
                    .equals(plmn)) {
                plmn = context.getResources()
                        .getString(com.android.internal.R.string.lockscreen_carrier_default);
            }
            str.append(plmn);
        } else if (showSpn && spn != null) {
            str.append(spn);
        }

        return str.toString();
    }
    /* @} */
}
