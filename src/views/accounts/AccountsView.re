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

module Mode = {
  type t =
    | Simple
    | Management;

  let is_simple =
    fun
    | Simple => true
    | Management => false;

  let is_management =
    fun
    | Simple => false
    | Management => true;

  let invert =
    fun
    | Simple => Management
    | Management => Simple;
};

module EditButton = {
  let styles =
    Style.(StyleSheet.create({"button": style(~marginTop=15.->dp, ())}));

  [@react.component]
  let make = (~mode, ~setMode) => {
    let onPress = _ => setMode(Mode.invert);
    <View style=styles##button>
      <ButtonAction
        onPress
        text={mode->Mode.is_management ? I18n.btn#done_ : I18n.btn#edit}
        icon={mode->Mode.is_management ? Icons.List.build : Icons.Edit.build}
      />
    </View>;
  };
};

module CreateAccountButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button": style(~marginLeft=(-6.)->dp, ~marginBottom=2.->dp, ()),
      })
    );

  [@react.component]
  let make = (~showOnboarding) => {
    <>
      <View style=styles##button>
        <ButtonAction
          onPress={_ => showOnboarding()}
          text=I18n.btn#create_or_import_secret
          icon=Icons.Account.build
          primary=true
        />
      </View>
    </>;
  };
};

module AccountsFlatList = {
  [@react.component]
  let make = (~token=?) => {
    let accounts = StoreContext.Accounts.useGetAll();
    <View>
      {accounts
       ->PublicKeyHash.Map.valuesToArray
       ->SortArray.stableSortBy(Account.compareName)
       ->Array.map(account =>
           <AccountRowItem key=(account.address :> string) account ?token />
         )
       ->React.array}
    </View>;
  };
};

module AccountsTreeList = {
  [@react.component]
  let make = () => {
    let secretsRequest = StoreContext.Secrets.useLoad();
    let accounts = StoreContext.Accounts.useGetAll();

    secretsRequest->ApiRequest.mapOrLoad(secrets => {
      let addressesInSecrets =
        secrets
        ->Array.map(secret => {
            let hdAddresses =
              secret.secret.addresses->Array.map(k => (k :> string));
            secret.secret.masterPublicKey
            ->Option.mapWithDefault(hdAddresses, legacyAddress => {
                hdAddresses->Array.concat([|(legacyAddress :> string)|])
              });
          })
        ->Array.reduce([||], (acc, arr) => acc->Array.concat(arr))
        ->Set.String.fromArray;

      let accountsNotInSecrets =
        accounts->PublicKeyHash.Map.keep((address, _account) => {
          !addressesInSecrets->Set.String.has((address :> string))
        });

      <>
        <View>
          {secrets
           ->Array.map(secret =>
               <SecretRowTree key={secret.index->string_of_int} secret />
             )
           ->React.array}
        </View>
        <View>
          {secrets
           ->Array.keepMap(secret =>
               secret.secret.masterPublicKey
               ->Option.map(legacyAddress => (secret, legacyAddress))
             )
           ->Array.map(((secret, legacyAddress)) =>
               <SecretRowTree.AccountImportedRowItem.Umami
                 key=(legacyAddress :> string)
                 address=legacyAddress
                 secret
               />
             )
           ->React.array}
        </View>
        <View>
          {accountsNotInSecrets
           ->PublicKeyHash.Map.valuesToArray
           ->Array.map(account =>
               <SecretRowTree.AccountImportedRowItem.Cli
                 key=(account.address :> string)
                 account
               />
             )
           ->React.array}
        </View>
      </>;
    });
  };
};

let styles =
  Style.(StyleSheet.create({"actionBar": style(~flexDirection=`row, ())}));

[@react.component]
let make = (~showOnboarding, ~mode, ~setMode) => {
  let resetSecrets = StoreContext.Secrets.useResetAll();
  let accountsRequest = StoreContext.Accounts.useRequest();
  let token = StoreContext.SelectedToken.useGet();

  let retryNetwork = ConfigContext.useRetryNetwork();

  <Page>
    {accountsRequest->ApiRequest.mapOrEmpty(_ => {
       <>
         <Page.Header
           right=
             {<>
                <RefreshButton
                  loading={accountsRequest->ApiRequest.isLoading}
                  onRefresh={() => {
                    resetSecrets();
                    retryNetwork();
                  }}
                />
                <EditButton mode setMode />
              </>}>
           <Typography.Headline style=Styles.title>
             I18n.title#accounts->React.string
           </Typography.Headline>
           {mode->Mode.is_management
              ? <BalanceTotal /> : <BalanceTotal.WithTokenSelector ?token />}
           <View style=styles##actionBar>
             {mode->Mode.is_management
                ? <CreateAccountButton showOnboarding /> : React.null}
           </View>
         </Page.Header>
         {mode->Mode.is_management
            ? <AccountsTreeList /> : <AccountsFlatList ?token />}
       </>
     })}
  </Page>;
};
