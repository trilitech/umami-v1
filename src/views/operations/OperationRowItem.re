open ReactNative;

%raw
"var Electron = window.require('electron');";
let electron = [%raw "Electron"];

module CellType =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=90.->dp, ()));
    ();
  });

module CellAmount =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=140.->dp, ()));
    ();
  });

module CellFee =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=80.->dp, ()));
    ();
  });

module CellAddress =
  Table.MakeCell({
    let style =
      Style.(style(~flexBasis=180.->dp, ~flexShrink=1., ~flexGrow=1., ()));
    ();
  });

module CellStatus =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=120.->dp, ~alignItems=`center, ()));
    ();
  });

module CellDate =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=160.->dp, ()));
    ();
  });

module CellAction =
  Table.MakeCell({
    let style =
      Style.(
        style(~flexBasis=30.->dp, ~minWidth=30.->dp, ~alignItems=`center, ())
      );
    ();
  });

let status = (operation: Operation.Read.t, currentLevel, config: ConfigFile.t) => {
  let (txt, colorStyle) =
    switch (operation.status) {
    | Mempool => (I18n.t#state_mempool, Some(`negative))
    | Chain =>
      let minConfirmations =
        config.confirmations->Option.getWithDefault(ConfigFile.confirmations);

      let currentConfirmations = currentLevel - operation.level;

      currentConfirmations > minConfirmations
        ? (I18n.t#state_confirmed, None)
        : (
          I18n.t#state_levels(currentConfirmations, minConfirmations),
          Some(`negative),
        );
    };

  <Typography.Body1 ?colorStyle> txt->React.string </Typography.Body1>;
};

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##operation == nextProps##operation
  );

let amount = (isToken, account, transaction: Operation.Business.Transaction.t) => {
  let colorStyle =
    account->Option.map((account: Account.t) =>
      account.address == transaction.destination ? `positive : `negative
    );

  let op = colorStyle == Some(`positive) ? "+" : "-";

  <CellAmount>
    {<Typography.Body1 ?colorStyle>
       {I18n.t#xtz_op_amount(op, transaction.amount->ProtocolXTZ.toString)
        ->React.string}
     </Typography.Body1>
     ->ReactUtils.onlyWhen(!isToken)}
  </CellAmount>;
};

[@react.component]
let make =
  memo((~operation: Operation.Read.t, ~currentLevel) => {
    let account = StoreContext.SelectedAccount.useGet();
    let aliases = StoreContext.Aliases.useGetAll();
    let tokens = StoreContext.Tokens.useGetAll();
    let config = ConfigContext.useSettings().config;

    <Table.Row>
      {switch (operation.payload) {
       | Business(business) =>
         switch (business.payload) {
         | Reveal(_reveal) =>
           <>
             <CellType>
               <Typography.Body1>
                 I18n.t#operation_reveal->React.string
               </Typography.Body1>
             </CellType>
             <CellAmount />
             <CellFee>
               <Typography.Body1>
                 {business.fee->ProtocolXTZ.toString->React.string}
               </Typography.Body1>
             </CellFee>
             <CellAddress />
             <CellAddress />
           </>
         | Transaction(transaction) =>
           let isToken = tokens->Map.String.has(transaction.destination);
           <>
             <CellType>
               <Typography.Body1>
                 I18n.t#operation_transaction->React.string
               </Typography.Body1>
             </CellType>
             {amount(isToken, account, transaction)}
             <CellFee>
               <Typography.Body1>
                 {business.fee->ProtocolXTZ.toString->React.string}
               </Typography.Body1>
             </CellFee>
             <CellAddress>
               {business.source
                ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
                ->Option.mapWithDefault(
                    <Typography.Address numberOfLines=1>
                      business.source->React.string
                    </Typography.Address>,
                    alias =>
                    <Typography.Body1 numberOfLines=1>
                      alias->React.string
                    </Typography.Body1>
                  )}
             </CellAddress>
             <CellAddress>
               {transaction.destination
                ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
                ->Option.mapWithDefault(
                    <Typography.Address numberOfLines=1>
                      transaction.destination->React.string
                    </Typography.Address>,
                    alias =>
                    <Typography.Body1 numberOfLines=1>
                      alias->React.string
                    </Typography.Body1>
                  )}
             </CellAddress>
           </>;
         | Origination(_origination) =>
           <>
             <CellType>
               <Typography.Body1>
                 I18n.t#operation_origination->React.string
               </Typography.Body1>
             </CellType>
             <CellAmount />
             <CellFee />
             <CellAddress />
             <CellAddress />
             <View />
           </>
         | Delegation(_delegation) =>
           <>
             <CellType>
               <Typography.Body1>
                 I18n.t#operation_delegation->React.string
               </Typography.Body1>
             </CellType>
             <CellAmount />
             <CellFee />
             <CellAddress />
             <CellAddress />
           </>
         | Unknown => React.null
         }
       }}
      <CellDate>
        <Typography.Body1>
          {operation.timestamp->DateFns.format("P pp")->React.string}
        </Typography.Body1>
      </CellDate>
      <CellStatus> {status(operation, currentLevel, config)} </CellStatus>
      <CellAction>
        <IconButton
          icon=Icons.OpenExternal.build
          onPress={_ => {
            electron##shell##openExternal(
              "https://edonet.tzkt.io/" ++ operation.hash,
            )
            ->ignore
          }}
        />
      </CellAction>
    </Table.Row>;
  });
