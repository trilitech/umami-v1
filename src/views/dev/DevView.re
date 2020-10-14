open Belt;
open ReactNative;
open Injection;

module AccountsAPI = API.Accounts(API.TezosClient);

let styles =
  Style.(
    StyleSheet.create({
      "main": style(~padding=8.->dp, ~backgroundColor="#ffffff", ()),
      "header":
        style(
          ~flex=1.,
          ~justifyContent=`spaceBetween,
          ~flexDirection=`row,
          ~margin=4.->dp,
          ~borderWidth=1.0,
          (),
        ),
      "section": style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()),
    })
  );

let toString = map =>
  map->Map.String.reduce("", (result, key, value) =>
    result ++ (result->String.length == 0 ? "" : "\n") ++ key ++ ": " ++ value
  );

[@react.component]
let make = (~setInjection, ~setAccounts, ~accounts) => {
  <View style=styles##main>
    <View style=styles##header> <BalanceOverview /> <NetworkSwitch /> </View>
    <TransactionForm
      onSubmit={(source, amount, destination) =>
        setInjection(_ =>
          Pending(
            Transaction(
              Injection.makeTransfer(~source, ~amount, ~destination, ()),
            ),
          )
        )
      }
    />
    <DelegateForm
      onSubmit={(source, delegate) =>
        AccountsAPI.add("delegate", delegate)
        ->Future.tapOk(_ =>
            AccountsAPI.get()->FutureEx.getOk(value => setAccounts(_ => value))
          )
        ->FutureEx.getOk(_ =>
            setInjection(_ =>
              Pending(Delegation({source, delegate: "delegate"}))
            )
          )
      }
    />
    <AccountCreationForm />
    <AccountRestorationForm />
    <AccountHDRestorationForm />
    <AccountDeletionForm />
    <Text style=styles##section> {accounts->toString->React.string} </Text>
    <OperationList />
  </View>;
};
