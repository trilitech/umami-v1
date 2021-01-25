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

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##operation == nextProps##operation
  );

let amount = (account, transaction: Operation.Business.Transaction.t) => {
  let colorStyle =
    account->Option.map((account: Account.t) =>
      account.address == transaction.destination ? `positive : `negative
    );

  let op = colorStyle == Some(`positive) ? "+" : "-";

  <CellAmount>
    <Typography.Body1 ?colorStyle>
      {I18n.t#xtz_op_amount(op, transaction.amount->ProtocolXTZ.toString)
       ->React.string}
    </Typography.Body1>
  </CellAmount>;
};

[@react.component]
let make =
  memo((~operation: Operation.Read.t) => {
    let account = StoreContext.SelectedAccount.useGet();
    let aliases = StoreContext.Aliases.useGetAll();

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
           <>
             <CellType>
               <Typography.Body1>
                 I18n.t#operation_transaction->React.string
               </Typography.Body1>
             </CellType>
             {amount(account, transaction)}
             <CellFee>
               <Typography.Body1>
                 {business.fee->ProtocolXTZ.toString->React.string}
               </Typography.Body1>
             </CellFee>
             <CellAddress>
               {business.source
                ->AliasHelpers.getAliasFromAddress(aliases)
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
                ->AliasHelpers.getAliasFromAddress(aliases)
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
           </>
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
      <CellStatus>
        <Typography.Body1>
          {switch (operation.status) {
           | Mempool => I18n.t#state_in_mempool
           | Chain => I18n.t#state_in_chain
           }}
          ->React.string
        </Typography.Body1>
      </CellStatus>
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
