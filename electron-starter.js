/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

'use strict'

// Import parts of electron to use
const { app, BrowserWindow, ipcMain, shell, Menu, MenuItem } = require('electron')
const path = require('path')
const url = require('url')
const { setAppMenu } = require('./src/AppMenu.bs')
const { autoUpdater } = require('electron-updater')

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow
let deeplinkURL

// Keep a reference for dev mode
let dev = process.env.NODE_ENV !== undefined && process.env.NODE_ENV === 'development'

// Broken:
// if (process.defaultApp || /[\\/]electron-prebuilt[\\/]/.test(process.execPath) || /[\\/]electron[\\/]/.test(process.execPath)) {
//   dev = true
// }

if (!app.requestSingleInstanceLock()) {
  app.quit()
  return
}

// Temporary fix broken high-dpi scale factor on Windows (125% scaling)
// info: https://github.com/electron/electron/issues/9691
if (process.platform === 'win32') {
  app.commandLine.appendSwitch('high-dpi-support', 'true')
  app.commandLine.appendSwitch('force-device-scale-factor', '1')
}

function createWindow() {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 1440,
    height: 1024,
    show: false,
    webPreferences: {

      // - `nodeIntegration: false` or `contextIsolation: true` :
      //   `Uncaught ReferenceError: global is not defined`
      // - `webSecurity: true` :
      //   Fetching external sites (e.g. umamiwallet.com) fails
      //   (blocked by CORS policy / net::ERR_FAILED)

      // Whether node integration is enabled. Default is false.
      nodeIntegration: true,

      // Whether to run Electron APIs and the specified preload script in a
      // separate JavaScript context.
      contextIsolation: false,

      // When false, it will disable the same-origin policy (usually using
      // testing websites by people), and set allowRunningInsecureContent to
      // true if this options has not been set by user.
      webSecurity: false,
      allowRunningInsecureContent: false

    }
  })

  require('@electron/remote/main').enable(mainWindow.webContents)

  // and load the index.html of the app.
  let indexPath

  if (dev && process.argv.indexOf('--noDevServer') === -1) {
    indexPath = url.format({
      protocol: 'http:',
      host: 'localhost:8080',
      pathname: 'index.html',
      slashes: true
    })
  } else {
    indexPath = url.format({
      protocol: 'file:',
      pathname: path.join(__dirname, 'build', 'index.html'),
      slashes: true
    })
  }

  mainWindow.loadURL(indexPath)

  // Don't show until we are ready and loaded
  mainWindow.once('ready-to-show', () => {
    mainWindow.show()

    // Open the DevTools automatically if developing
    if (dev) {
      const { default: installExtension, REACT_DEVELOPER_TOOLS } = require('electron-devtools-installer')

      installExtension(REACT_DEVELOPER_TOOLS)
        .catch(err => console.log('Error loading React DevTools: ', err))
      mainWindow.webContents.openDevTools()
    }

    if (deeplinkURL) {
      mainWindow.webContents.send('deeplinkURL', deeplinkURL)
      deeplinkURL = null
    } else if (process.platform == 'win32' || process.platform === "linux") {
      // Protocol handler for windows & linux
      const argv = process.argv;
      const index = argv.findIndex(arg => arg.startsWith("umami://"));
      if (index !== -1) {
        mainWindow.webContents.send('deeplinkURL', argv[index])
      }
    }
  })

  mainWindow.webContents.setWindowOpenHandler((details) => {
    console.log(details);
    if (details.frameName === '_blank') {
      require('electron').shell.openExternal(details.url);
      return { action: 'deny' };
    } else {
      return { action: 'allow' };
    }
  });

  // Emitted when the window is closed.
  mainWindow.on('closed', () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null
  })
}

app.commandLine.appendSwitch('disable-features', 'OutOfBlinkCors');

if (!app.isDefaultProtocolClient('umami')) {
  // Define custom protocol handler. Deep linking works on packaged versions of the application!
  app.setAsDefaultProtocolClient('umami')
}

let setupAutoUpdate = async () => {

  let downloading = false;

  let checkForUpdates = () => {
    return autoUpdater.checkForUpdates()
      .catch(err => mainWindow.webContents.send('check-update-error', err.toString()))
  };

  let launchAutomaticUpdates = () => {
    checkForUpdates();
    return setInterval(checkForUpdates, 720000); // 720000 ms = 12 minutes
  };

  let config = await mainWindow.webContents.executeJavaScript(`localStorage.getItem("Config")`);
  config = JSON.parse(config);
  autoUpdater.autoDownload = config.autoUpdates;
  let timer = null;

  if (config.autoUpdates) {
    timer = launchAutomaticUpdates();
  }

  ipcMain.on("check-and-ask", _ => {
    let autodown_prev = autoUpdater.autoDownload;
    autoUpdater.autoDownload = false;
    checkForUpdates().finally(_ => autoUpdater.autoDownload = autodown_prev);
  });

  ipcMain.on("download-update", _ => {
    if (!downloading){
      downloading = true;
      autoUpdater.downloadUpdate();
    }
  });

  ipcMain.on("setAutoDownload", (_, msg) => {
    let autoDownload = JSON.parse(msg);
    autoUpdater.autoDownload = autoDownload;
    if (timer != null) {
      clearInterval(timer);
      timer = null;
    }
    if (autoDownload) {
      timer = launchAutomaticUpdates();
    }
  });

  ipcMain.on('quit-and-install', _ => autoUpdater.quitAndInstall());

  autoUpdater.on("download-progress", (progressObj) => mainWindow.webContents.send('download-progress', progressObj.percent));
  autoUpdater.on("update-not-available", _ => mainWindow.webContents.send('no-update', ""));
  autoUpdater.on('update-available', _ => mainWindow.webContents.send('update-available', ""));
  autoUpdater.on('update-downloaded', _ => {
    downloading = false;
    mainWindow.webContents.send('update-downloaded', "");
  });

  autoUpdater.on('error', err => {
    console.log("oops, an error: ", err);
    mainWindow.webContents.send('update-error', err.toString());
  });

};

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', () => {

  // Do not include this in createWindow() function
  // (issue 714: macOS @electron/remote initialized multiple times)
  require('@electron/remote/main').initialize();

  setAppMenu();
  createWindow();

  // Auto-update works only on appimages on linux, otherwise anything on
  // windows and macos
  if (process.plateform != 'linux' || process.env.APPIMAGE !== undefined) {
    setupAutoUpdate();
  }
})

// Quit when all windows are closed.
app.on('window-all-closed', () => {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit()
  }
})



app.on('activate', () => {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    createWindow()
  }
})

app.on('second-instance', (event, argv, cwd) => {
  if (mainWindow) {
    if (mainWindow.isMinimized()) {
      mainWindow.restore()
    }
    mainWindow.focus()
    // Protocol handler for win32
    // argv: An array of the second instance’s (command line / deep linked) arguments
    if (process.platform == 'win32' || process.platform === "linux") {
      // Protocol handler for windows & linux
      const index = argv.findIndex(arg => arg.startsWith("umami://"));
      if (index !== -1) {
        mainWindow.webContents.send('deeplinkURL', argv[index])
      }
    }
  } else {
    createWindow()
  }
})

app.on('will-finish-launching', () => {
  // Protocol handler for osx
  app.on('open-url', function (event, url) {
    event.preventDefault()
    //logEverywhere('open-url# ' + url)
    if (mainWindow) {
      mainWindow.webContents.send('deeplinkURL', url)
    } else {
      deeplinkURL = url
      createWindow()
    }
  })
})

// Log both at dev console and at running node console instance
function logEverywhere(s) {
  console.log(s)
  if (mainWindow && mainWindow.webContents) {
    mainWindow.webContents.executeJavaScript(`console.log("${s}")`)
  }
}
