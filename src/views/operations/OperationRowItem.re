open ReactNative;

let baseCellStyle = Style.(style(~flexShrink=0., ~marginRight=24.->dp, ()));
let styles =
  Style.(
    StyleSheet.create({
      "borderSpacer": style(~width=20.->dp, ()),
      "cellType":
        StyleSheet.flatten([|baseCellStyle, style(~flexBasis=90.->dp, ())|]),
      "cellAmount":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=120.->dp, ()),
        |]),
      "cellFee":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=120.->dp, ()),
        |]),
      "cellAddress":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=180.->dp, ~flexShrink=1., ~flexGrow=1., ()),
        |]),
      "cellStatus":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=120.->dp, ~alignItems=`center, ()),
        |]),
      "cellDate":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=220.->dp, ()),
        |]),
    })
  );

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##operation == nextProps##operation
  );

let amount = (account, transaction: Operation.Business.Transaction.t) => {
  let colorStyle =
    account->Belt.Option.map((account: Account.t) =>
      account.address == transaction.destination ? `valid : `error
    );

  let op = colorStyle == Some(`valid) ? "+" : "-";

  <View style=styles##cellAmount>
    <Typography.Body1 ?colorStyle>
      op->React.string
      " "->React.string
      {transaction.amount->BusinessUtils.formatMilliXTZ->React.string}
      " "->React.string
      BusinessUtils.xtz->React.string
    </Typography.Body1>
  </View>;
};

[@react.component]
let make =
  memo((~operation: Operation.t) => {
    let account = StoreContext.useAccount();

    let accounts = StoreContext.useAccounts();

    <RowItem.Bordered height=48.>
      {_ => {
         <>
           <View style=styles##borderSpacer />
           {switch (operation.payload) {
            | Business(business) =>
              switch (business.payload) {
              | Reveal(_reveal) =>
                <>
                  <View style=styles##cellType>
                    <Typography.Body1>
                      "Reveal"->React.string
                    </Typography.Body1>
                  </View>
                  <View style=styles##cellAmount />
                  <View style=styles##cellFee>
                    <Typography.Body1>
                      {business.fee->BusinessUtils.formatMilliXTZ->React.string}
                    </Typography.Body1>
                  </View>
                  <View style=styles##cellAddress />
                  <View style=styles##cellAddress />
                </>
              | Transaction(transaction) =>
                <>
                  <View style=styles##cellType>
                    <Typography.Body1>
                      "Transaction"->React.string
                    </Typography.Body1>
                  </View>
                  {amount(account, transaction)}
                  <View style=styles##cellFee>
                    <Typography.Body1>
                      {business.fee->BusinessUtils.formatMilliXTZ->React.string}
                    </Typography.Body1>
                  </View>
                  <View style=styles##cellAddress>
                    <Typography.Body1 numberOfLines=1>
                      {StoreContext.getAlias(accounts, business.source)
                       ->React.string}
                    </Typography.Body1>
                  </View>
                  <View style=styles##cellAddress>
                    <Typography.Body1 numberOfLines=1>
                      {StoreContext.getAlias(
                         accounts,
                         transaction.destination,
                       )
                       ->React.string}
                    </Typography.Body1>
                  </View>
                </>
              | Origination(_origination) =>
                <>
                  <View style=styles##cellType>
                    <Typography.Body1>
                      "Origination"->React.string
                    </Typography.Body1>
                  </View>
                  <View style=styles##cellAmount />
                  <View style=styles##cellFee />
                  <View style=styles##cellAddress />
                  <View style=styles##cellAddress />
                  <View />
                </>
              | Delegation(_delegation) =>
                <>
                  <View style=styles##cellType>
                    <Typography.Body1>
                      "Delegation"->React.string
                    </Typography.Body1>
                  </View>
                  <View style=styles##cellAmount />
                  <View style=styles##cellFee />
                  <View style=styles##cellAddress />
                  <View style=styles##cellAddress />
                </>
              | Unknown => React.null
              }
            }}
           <View style=styles##cellDate>
             <Typography.Body1>
               {operation.timestamp->Js.Date.toLocaleString->React.string}
             </Typography.Body1>
           </View>
           <View style=styles##cellStatus>
             <Typography.Body1>
               {switch (operation.status) {
                | Mempool => "in mempool"
                | Chain => "in chain"
                }}
               ->React.string
             </Typography.Body1>
           </View>
         </>;
       }}
    </RowItem.Bordered>;
  });
