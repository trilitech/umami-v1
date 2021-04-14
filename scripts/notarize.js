const { notarize } = require('electron-notarize');

exports.default = async function notarizing(context) {
  const { electronPlatformName, appOutDir } = context;  
  if (electronPlatformName !== 'darwin') {
    return;
  }

  const appName = context.packager.appInfo.productFilename;
  const appleId = '<your apple ID>';
  const appleIdPassword = '<your app-specific password>'

  return await notarize({
    appBundleId: 'com.nomadic-labs.umami',
    appPath: `${appOutDir}/${appName}.app`,
    appleId: `${appleId}`,
    appleIdPassword: `${appleIdPassword}`,
  });
};
