open SettingsComponents;
open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button": style(~marginTop=30.->dp, ()),
      "second_button": style(~marginTop=8.->dp, ()),
      "dropdown_button": style(~marginLeft="auto", ~marginRight="0", ()),
      "banner":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~width="100%",
          ~borderRadius=6.,
          (),
        ),
      "right_button":
        style(
          ~marginLeft="auto",
          ~right="0",
          ~paddingLeft=20.->dp,
          ~paddingRight=16.->dp,
          (),
        ),
      "max_content": style(~width="max-content", ()),
      "banner_icon": style(~marginLeft=10.->dp, ()),
      "banner_text_with_icon":
        style(
          ~marginLeft=10.->dp,
          ~marginRight=22.->dp,
          ~paddingTop=10.->dp,
          ~paddingBottom=10.->dp,
          (),
        ),
      "banner_text":
        style(
          ~marginLeft=22.->dp,
          ~marginRight=22.->dp,
          ~paddingTop=10.->dp,
          ~paddingBottom=10.->dp,
          (),
        ),
      "flex_elt": style(~flex=1., ()),
    })
  );

type updateState =
  | Checking
  | UpdateAvailable
  | UpToDate
  | Downloading(float)
  | Downloaded
  | Unchecked
  | Error(string)
  | CheckUpdateError(string);

module CheckButton = {
  [@react.component]
  let make = (~disabled=?, ~style=?, ~checkForUpdates) => {
    <View style={Style.arrayOption([|Some(styles##max_content), style|])}>
      <ButtonAction
        text=I18n.Label.check_for_updates
        icon=Icons.Refresh.build
        ?disabled
        onPress={_ => checkForUpdates()}
      />
    </View>;
  };
};

module Banner = {
  [@react.component]
  let make = (~data, ~onClose=?) => {
    let theme = ThemeContext.useTheme();

    let closeBtn =
      switch (onClose) {
      | None => React.null
      | Some(onClose) =>
        <View style=styles##right_button>
          <IconButton
            icon=Icons.Close.build
            onPress={_ => onClose(_ => Unchecked)}
          />
        </View>
      };

    <View
      style=Style.(
        array([|
          style(~backgroundColor=theme.colors.textPrimary, ()),
          styles##banner,
        |])
      )>
      <View
        style=Style.(
          array([|
            style(~backgroundColor=theme.colors.backgroundMediumEmphasis, ()),
            styles##banner,
          |])
        )>
        data
        closeBtn
      </View>
    </View>;
  };
};

module Error = {
  [@react.component]
  let make = (~msg, ~logMsg, ~logToast, ~checkForUpdates, ~setStep) => {
    let log = Logs.log(~origin=Logs.Update, ~kind=Logs.Error, logMsg);
    React.useEffect0(_ => {
      logToast(log);
      None;
    });
    let theme = ThemeContext.useTheme();
    let check = _ => checkForUpdates();
    let data =
      <>
        <Icons.Info
          size=30.
          style=styles##banner_icon
          color={theme.colors.textHighEmphasis}
        />
        <Typography.Body1 style=styles##banner_text_with_icon>
          msg->React.string
        </Typography.Body1>
        <Buttons.FormPrimary
          text=I18n.Btn.try_again
          onPress=check
          style=styles##flex_elt
        />
      </>;

    <>
      <CheckButton checkForUpdates />
      <Banner data onClose={_ => setStep(_ => Unchecked)} />
    </>;
  };
};

module UpdateAvailable = {
  [@react.component]
  let make = (~checkForUpdates, ~download, ~setStep) => {
    let theme = ThemeContext.useTheme();
    let data =
      <>
        // Add an I icon here
        <Icons.Info
          size=30.
          style=styles##banner_icon
          color={theme.colors.textHighEmphasis}
        />
        <Typography.Body1 style=styles##banner_text_with_icon>
          I18n.update_available->React.string
        </Typography.Body1>
        <Buttons.FormPrimary
          text=I18n.Btn.download
          onPress={_ => download()}
          style=styles##flex_elt
        />
      </>;
    <>
      <CheckButton checkForUpdates />
      <Banner data onClose={_ => setStep(_ => Unchecked)} />
    </>;
  };
};

module Downloading = {
  [@react.component]
  let make = (~percentage, ~checkForUpdates) => {
    let theme = ThemeContext.useTheme();
    let percentage = Int.toString(percentage->Int.fromFloat);
    let data =
      <>
        <ActivityIndicator
          animating=true
          size=ActivityIndicator_Size.small
          color={theme.colors.textPositive}
          style=styles##banner_icon
        />
        <Typography.Body1 style=styles##banner_text_with_icon>
          {I18n.downloading_update(percentage)->React.string}
        </Typography.Body1>
      </>;
    <> <CheckButton checkForUpdates /> <Banner data /> </>;
  };
};

module UpToDate = {
  [@react.component]
  let make = (~checkForUpdates, ~setStep) => {
    let onClose = _ => setStep(_ => Unchecked);
    let data =
      <>
        <Typography.Body1 style=styles##banner_text>
          {I18n.Title.wallet_up_to_date(System.getVersion())->React.string}
        </Typography.Body1>
      </>;
    <> <CheckButton checkForUpdates /> <Banner data onClose /> </>;
  };
};

module Checking = {
  [@react.component]
  let make = (~checkForUpdates) => {
    let theme = ThemeContext.useTheme();
    <View style={Style.style(~display=`flex, ~flexDirection=`row, ())}>
      <CheckButton disabled=true checkForUpdates />
      <ActivityIndicator animating=true color={theme.colors.textMaxEmphasis} />
    </View>;
  };
};

module DownloadComplete = {
  [@react.component]
  let make = (~checkForUpdates, ~setStep) => {
    let theme = ThemeContext.useTheme();

    let data =
      <>
        <Icons.CheckOutline
          size=22.
          color={theme.colors.valid}
          style=styles##banner_icon
        />
        <Typography.Body1 style=styles##banner_text_with_icon>
          I18n.download_complete->React.string
        </Typography.Body1>
        <Buttons.FormPrimary
          colorStyle=`primary
          text=I18n.Btn.install_and_restart_now
          onPress={_ => IPC.send("quit-and-install", "")}
          style=styles##flex_elt
        />
        <Buttons.FormPrimary
          colorStyle=`primary
          text=I18n.Btn.ill_do_it_later
          onPress={_ => setStep(_ => Unchecked)}
          style=styles##flex_elt
        />
      </>;
    <> <CheckButton checkForUpdates /> <Banner data /> </>;
  };
};

[@react.component]
let make = () => {
  let writeConf = ConfigContext.useWrite();
  let configFile = ConfigContext.useFile();
  let logToast = LogsContext.useToast();

  let (step, setStep) = React.useState(_ => Unchecked);

  React.useEffect0(() => {
    IPC.on("no-update", (_, _) => setStep(_ => UpToDate));
    IPC.on("update-downloaded", (_, _) => {setStep(_ => Downloaded)});
    IPC.on("update-available", (_, _) => setStep(_ => UpdateAvailable));
    IPC.on("download-progress", (_, percent) => {
      setStep(_ => Downloading(percent))
    });
    IPC.on("update-error", (_, msg) => setStep(_ => Error(msg)));
    IPC.on("check-update-error", (_, msg) =>
      setStep(_ => CheckUpdateError(msg))
    );
    None;
  });

  let checkForUpdates = () => {
    setStep(_ => Checking);
    IPC.send("check-and-ask", "");
  };

  let download = () => {
    IPC.send("download-update", "");
    setStep(_ => Downloading(0.));
  };

  let (autoUpdates, setCheckUpdates) =
    React.useState(() => configFile.autoUpdates);

  let writeCheckUpdates = f => {
    let autoUpdates = configFile.autoUpdates;
    let autoUpdates = f(autoUpdates);
    setCheckUpdates(_ => autoUpdates);
    IPC.send("setAutoDownload", string_of_bool(autoUpdates));
    writeConf(c => {...c, autoUpdates});
  };
  <>
    <Block title="App update">
      <View style={Style.style(~width="100%", ())}>
        <View style={Style.style(~textAlign=`center, ())}>
          {switch (step) {
           | Checking => <Checking checkForUpdates />
           | Downloading(percentage) =>
             <Downloading percentage checkForUpdates />
           | Downloaded => <DownloadComplete checkForUpdates setStep />
           | Error(logMsg) =>
             let msg = I18n.Errors.unable_to_download;
             <Error msg logMsg logToast checkForUpdates setStep />;
           | CheckUpdateError(logMsg) =>
             let msg = I18n.Errors.error_while_checking_updates;
             <Error msg logMsg logToast checkForUpdates setStep />;
           | UpdateAvailable =>
             <UpdateAvailable checkForUpdates download setStep />
           | UpToDate => <UpToDate checkForUpdates setStep />
           | Unchecked => <CheckButton checkForUpdates />
           }}
        </View>
        <View style=styles##max_content>
          <SwitchItem
            label=I18n.Label.automatically_download_updates
            value=autoUpdates
            setValue={_ => writeCheckUpdates(prev => !prev)}
          />
        </View>
      </View>
    </Block>
  </>;
};
