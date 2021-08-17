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

open ReactNative;

let onbStyles = FormStyles.onboarding;

type step =
  | StepChecklist
  | StepInitLedger(InitLedgerView.ledgerState)
  | StepAdvancedOptions(Wallet.Ledger.masterKey)
  | StepAccounts(Wallet.Ledger.masterKey);

module ChecklistView = {
  let styles =
    Style.(
      StyleSheet.create({
        "check": style(~marginRight=11.->dp, ()),
        "itemSpacing": style(~marginTop=19.->dp, ()),
        "itemContent": style(~display=`flex, ~flexDirection=`row, ()),
      })
    );

  module Item = {
    [@react.component]
    let make = (~text, ~first=false) => {
      let theme = ThemeContext.useTheme();

      <View
        style=Style.(
          arrayOption([|
            Some(styles##itemContent),
            first ? None : Some(styles##itemSpacing),
          |])
        )>
        <Icons.Checkmark
          size=20.
          style=styles##check
          color={theme.colors.iconHighEmphasis}
        />
        <Typography.Subtitle2> text->React.string </Typography.Subtitle2>
      </View>;
    };
  };

  [@react.component]
  let make = (~next) => {
    <>
      <Typography.Headline style=onbStyles##title>
        I18n.title#hardware_wallet_connect->React.string
      </Typography.Headline>
      <Typography.Overline1 style=onbStyles##title>
        I18n.title#hardware_wallet_check_follow->React.string
      </Typography.Overline1>
      <Typography.Body2 style=onbStyles##title>
        I18n.expl#hardware_wallet_check_complete_steps->React.string
      </Typography.Body2>
      <View style=FormStyles.section##spacing>
        {[|
           I18n.expl#hardware_wallet_check_plug,
           I18n.expl#hardware_wallet_check_unlock,
           I18n.expl#hardware_wallet_check_firmware,
           I18n.expl#hardware_wallet_check_app,
           I18n.expl#hardware_wallet_check_click,
         |]
         ->Array.mapWithIndex((i, text) =>
             <Item key={i->Int.toString} first={i == 0} text />
           )
         ->React.array}
      </View>
      <Buttons.SubmitPrimary
        text=I18n.btn#export_public_key
        onPress={_ => next()}
        style=FormStyles.section##submitSpacing
      />
    </>;
  };
};

module AdvancedOptionsView = {
  module SchemeSelector = {
    let render = s => {
      let theme = ThemeContext.useTheme();
      let kind =
        s
        ->Wallet.Ledger.implicitFromScheme
        ->Implicit
        ->Wallet.Ledger.kindToString;

      <View style=FormStyles.selector##item>
        <Typography.Contract
          style=Style.(
            style(
              ~backgroundColor=theme.colors.stateDisabled,
              ~marginRight=10.->dp,
              ~paddingVertical=4.->dp,
              ~paddingHorizontal=16.->dp,
              ~borderRadius=50.,
              (),
            )
          )>
          kind->React.string
        </Typography.Contract>
        <Typography.Body1>
          {s->Wallet.Ledger.schemeToString->React.string}
        </Typography.Body1>
      </View>;
    };

    let renderButton = (s, _hasError) => {
      let s = s->Option.getWithDefault(Wallet.Ledger.ED25519);
      <View style=FormStyles.selector##button> {render(s)} </View>;
    };

    [@react.component]
    let make = (~value, ~onValueChange) => {
      <>
        <FormLabel
          style=FormStyles.selector##label
          label=I18n.label#derivation_scheme
        />
        <Selector
          items=Wallet.Ledger.([|ED25519, SECP256K1, P256|])
          getItemKey=Wallet.Ledger.schemeToString
          renderItem=render
          selectedValueKey={value->Wallet.Ledger.schemeToString}
          onValueChange
          renderButton
          keyPopover="MnemonicsFormatSelector"
        />
      </>;
    };
  };

  open SelectDerivationPathView.SelectDerivationPathForm;

  [@react.component]
  let make =
      (
        ~schemeState as (globalScheme, setGlobalScheme),
        ~pathState as (path, setPath),
        ~next,
      ) => {
    let (localScheme, setScheme) = React.useState(() => globalScheme);

    let form: api =
      SelectDerivationPathView.form(
        ~custom=true,
        ~setDerivationPath=setPath,
        ~next=
          state => {
            setGlobalScheme(_ => localScheme);
            setPath(_ =>
              state.values.customDerivationPath
              ->DerivationPath.Pattern.fromString
              ->Result.getExn
            );
            next();
          },
        ~derivationPath=path,
      );

    <>
      <Typography.Headline style=onbStyles##title>
        I18n.title#hardware_wallet_connect->React.string
      </Typography.Headline>
      <Typography.Body1 style=onbStyles##title>
        I18n.expl#hardware_wallet_advopt->React.string
      </Typography.Body1>
      <View style=FormStyles.section##spacing>
        <SchemeSelector
          value=localScheme
          onValueChange={v => setScheme(_ => v)}
        />
        <View style=FormStyles.section##spacing>
          <FormLabel
            style=FormStyles.selector##label
            label=I18n.label#derivation_path
          />
          <SelectDerivationPathView.DerivationPathInput form />
        </View>
      </View>
      <Buttons.SubmitPrimary
        style=FormStyles.section##submitSpacing
        text=I18n.btn#verify_accounts
        onPress={_ => form.submit()}
      />
    </>;
  };
};

[@react.component]
let make = (~closeAction) => {
  let config = ConfigContext.useContent();
  let (step, setStep) = React.useState(() => StepChecklist);
  let addLog = LogsContext.useAdd();

  let (_importLedgerRequest, importLedger) =
    StoreContext.Secrets.useLedgerImport();

  let secrets = StoreContext.Secrets.useGetAll();
  let existingSecrets = secrets->Array.length > 0;
  let existingLedgerSecretsCount =
    secrets->Array.keep(s => s.secret.kind == Ledger)->Array.length;

  let importLedger = p =>
    System.Client.initDir(config->ConfigUtils.baseDir)
    ->Future.mapError(e => ErrorHandler.(e->File->Wallet))
    ->Future.flatMapOk(() => importLedger(p))
    ->Future.tapOk(_ => {closeAction()})
    ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
        I18n.t#account_created
      )
    ->ignore;

  let closing =
    switch (step) {
    | StepInitLedger(`Denied(_))
    | StepChecklist => ModalFormView.Close(closeAction)->Some
    | StepInitLedger(_) => None
    | StepAdvancedOptions(_)
    | StepAccounts(_) =>
      existingSecrets ? ModalFormView.Close(closeAction)->Some : None
    };

  let back =
    switch (step) {
    | StepAdvancedOptions(tr) => Some(() => setStep(_ => StepAccounts(tr)))
    | StepInitLedger(_)
    | StepChecklist
    | StepAccounts(_) => None
    };

  let (scheme, _) as schemeState =
    React.useState(() => Wallet.Ledger.ED25519);
  let (path, _) as pathState =
    React.useState(() => DerivationPath.Pattern.(default->fromTezosBip44));

  let (_, setDerivationChanged) as derivationChangedState =
    React.useState(() => true);

  React.useEffect2(
    () => {
      setDerivationChanged(_ => true);
      None;
    },
    (scheme, path),
  );

  let onEndChecklist = () => {
    setStep(_ => StepInitLedger(`Loading));
    LedgerAPI.init(~timeout=5000, ())
    ->Future.flatMapOk(tr => {
        setStep(_ => StepInitLedger(`Found));
        LedgerAPI.getFirstKey(~prompt=true, tr)
        ->Future.flatMapOk(_ => {
            setStep(_ => StepInitLedger(`Found));
            LedgerAPI.getMasterKey(~prompt=false, tr);
          });
      })
    ->Future.flatMapOk(masterKey => {
        setStep(_ => StepInitLedger(`Confirmed));
        FutureEx.timeout(1500)
        ->Future.mapOk(() => setStep(_ => StepAccounts(masterKey)));
      })
    ->FutureEx.getError(e => setStep(_ => StepInitLedger(`Denied(e))));
  };

  let (accounts, setAccounts) = React.useState(() => []);

  let onFoundKey = (i, address) =>
    setAccounts(accounts =>
      if (accounts->List.has(address, (==))) {
        accounts;
      } else {
        let accounts = i == 0 ? [] : accounts;

        [address, ...accounts];
      }
    );

  let submitAccounts = (ledgerMasterKey, ()) => {
    let name =
      "Ledger Secret " ++ (existingLedgerSecretsCount + 1)->Int.toString;

    importLedger(
      SecretApiRequest.{
        name,
        derivationPath: path,
        derivationScheme: scheme,
        accountsNumber: List.length(accounts),
        timeout: 1000,
        ledgerMasterKey,
      },
    );
  };

  <ModalFormView back ?closing>
    {switch (step) {
     | StepInitLedger(status) =>
       <InitLedgerView status retry=onEndChecklist />

     | StepChecklist => <ChecklistView next=onEndChecklist />
     | StepAccounts(mk) =>
       <ScannedAccountsView
         scan={WalletAPI.Accounts.Scan.runStreamLedger(
           ~config,
           ~onFoundKey,
           ~startIndex=0,
         )}
         derivationChangedState
         path
         scheme
         accounts
         next={submitAccounts(mk)}
         nextAdvancedOptions={
           Some(() => setStep(_ => StepAdvancedOptions(mk)))
         }
       />
     | StepAdvancedOptions(mk) =>
       <AdvancedOptionsView
         schemeState
         pathState
         next={() => setStep(_ => StepAccounts(mk))}
       />
     }}
  </ModalFormView>;
};
