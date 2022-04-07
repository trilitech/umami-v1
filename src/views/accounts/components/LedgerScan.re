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

type step =
  | StepInit(InitLedgerView.ledgerState)
  | StepAccounts(PublicKeyHash.t);

[@react.component]
let make = (~closeAction, ~index, ~secret) => {
  let theme = ThemeContext.useTheme();
  let config = ConfigContext.useContent();
  let addLog = LogsContext.useAdd();
  let (status, setStatus) = React.useState(() => StepInit(`Loading));
  let (accounts, setAccounts) = React.useState(() => []);
  let derivationChangedState = React.useState(() => true);

  let (_, ledgerScan) = StoreContext.Secrets.useLedgerScan();

  let onFoundKey = (~start, i, address) =>
    setAccounts(accounts =>
      if (accounts->List.has(address, (==))) {
        accounts;
      } else {
        let accounts = i == start ? [] : accounts;

        [address, ...accounts];
      }
    );

  let ledgerInteract = () => {
    setStatus(_ => StepInit(`Loading));
    LedgerAPI.init(~timeout=5000, ())
    ->Promise.flatMapOk(tr => {
        setStatus(_ => StepInit(`Found));
        LedgerAPI.getFirstKey(~prompt=true, tr)
        ->Promise.flatMapOk(_ => {
            setStatus(_ => StepInit(`Found));
            LedgerAPI.getMasterKey(~prompt=false, tr);
          });
      })
    ->Promise.flatMapOk(masterKey => {
        setStatus(_ => StepInit(`Confirmed));
        Promise.timeout(1500)
        ->Promise.mapOk(() => setStatus(_ => StepAccounts(masterKey)));
      })
    ->Promise.getError(e => setStatus(_ => StepInit(`Denied(e))));
  };

  let submitAccounts = (ledgerMasterKey, ()) => {
    ledgerScan(
      SecretApiRequest.{
        index,
        accountsNumber: List.length(accounts),
        timeout: 1000,
        ledgerMasterKey,
      },
    )
    ->ApiRequest.logOk(addLog(true), Logs.Account, _ => I18n.account_created)
    ->Promise.getOk(_ => {closeAction()});
  };

  React.useEffect0(() => {
    ledgerInteract();
    None;
  });

  let style =
    Style.(
      style(
        ~backgroundColor=theme.colors.stateDisabled,
        ~marginTop=20.->dp,
        ~borderRadius=4.,
        ~padding=32.->dp,
        ~minHeight=282.->dp,
        (),
      )
    );

  <ModalFormView
    title=I18n.Title.scan closing={ModalFormView.Close(closeAction)}>
    {switch (status) {
     | StepInit(status) =>
       <>
         <Typography.Overline1
           fontSize=19. style=FormStyles.headerWithoutMarginBottom>
           I18n.Title.hardware_confirm_pkh->React.string
         </Typography.Overline1>
         <Typography.Body2
           fontSize=14. style=FormStyles.headerWithoutMarginBottom>
           I18n.Expl.scan->React.string
         </Typography.Body2>
         <InitLedgerView style status retry=ledgerInteract inline=true />
       </>
     | StepAccounts(mk) =>
       <ScannedAccountsView
         scan={LedgerWalletAPI.runStreamedScan(
           ~config,
           ~startIndex=secret.Secret.addresses->Js.Array.length,
           ~onFoundKey=
             onFoundKey(~start=secret.Secret.addresses->Js.Array.length),
         )}
         derivationChangedState
         path={secret.Secret.derivationPath}
         scheme={secret.Secret.derivationScheme}
         accounts={accounts->List.reverse}
         next={submitAccounts(mk)}
         nextAdvancedOptions=None
       />
     }}
  </ModalFormView>;
};
