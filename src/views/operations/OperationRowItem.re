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
    let style = Style.(style(~flexBasis=86.->dp, ()));
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
    let style = Style.(style(~flexBasis=100.->dp, ()));
    ();
  });

module CellDate =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=180.->dp, ()));
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

let styles =
  Style.(
    StyleSheet.create({
      "rawAddressContainer":
        style(~display=`flex, ~flexDirection=`row, ~alignItems=`center, ()),
    })
  );

module AddContactButton = {
  [@react.component]
  let make = (~address) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <IconButton icon=Icons.AddContact.build onPress />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ContactFormView initAddress=address action=Create closeAction />
      </ModalAction>
    </>;
  };
};

let rawUnknownAddress = address => {
  <View style=styles##rawAddressContainer>
    <Typography.Address numberOfLines=1>
      address->React.string
    </Typography.Address>
    <AddContactButton address />
  </View>;
};

let getContactOrRaw = (aliases, tokens, address) => {
  address
  ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
  ->Option.mapWithDefault(rawUnknownAddress(address), alias =>
      <Typography.Body1 numberOfLines=1>
        alias->React.string
      </Typography.Body1>
    );
};

let status = (operation: Operation.Read.t, currentLevel, config: ConfigFile.t) => {
  let (txt, colorStyle) =
    switch (operation.status) {
    | Mempool => (I18n.t#state_mempool, Some(`negative))
    | Chain =>
      let minConfirmations =
        config.confirmations
        ->Option.getWithDefault(ConfigFile.Default.confirmations);

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
  React.memoCustomCompareProps(
    component,
    (prevPros, nextProps) => {
      let currentConfirmations =
        prevPros##currentLevel - prevPros##operation.Operation.Read.level;

      currentConfirmations > 5
        ? prevPros##operation == nextProps##operation
        : prevPros##operation ==
          nextProps##operation
          &&
          prevPros##currentLevel ==
          nextProps##currentLevel;
    },
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
    let settings = SdkContext.useSettings();
    let addToast = LogsContext.useToast();

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
                 {I18n.t#xtz_amount(business.fee->ProtocolXTZ.toString)
                  ->React.string}
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
                 {I18n.t#xtz_amount(business.fee->ProtocolXTZ.toString)
                  ->React.string}
               </Typography.Body1>
             </CellFee>
             <CellAddress>
               {business.source
                ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
                ->Option.mapWithDefault(
                    rawUnknownAddress(business.source), alias =>
                    <Typography.Body1 numberOfLines=1>
                      alias->React.string
                    </Typography.Body1>
                  )}
             </CellAddress>
             <CellAddress>
               {getContactOrRaw(aliases, tokens, transaction.destination)}
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
         | Delegation(delegation) =>
           <>
             <CellType>
               <Typography.Body1>
                 I18n.t#operation_delegation->React.string
               </Typography.Body1>
             </CellType>
             <CellAmount />
             <CellFee>
               <Typography.Body1>
                 {I18n.t#xtz_amount(business.fee->ProtocolXTZ.toString)
                  ->React.string}
               </Typography.Body1>
             </CellFee>
             <CellAddress>
               {getContactOrRaw(aliases, tokens, business.source)}
             </CellAddress>
             {delegation.delegate
              ->Option.mapWithDefault(
                  <CellAddress>
                    <Typography.Body1 numberOfLines=1>
                      I18n.t#delegation_removal->React.string
                    </Typography.Body1>
                  </CellAddress>,
                  d =>
                  <CellAddress>
                    {getContactOrRaw(aliases, tokens, d)}
                  </CellAddress>
                )}
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
        {status(operation, currentLevel, settings.config)}
      </CellStatus>
      <CellAction>
        <IconButton
          size=34.
          icon=Icons.OpenExternal.build
          onPress={_ => {
            switch (AppSettings.getExternalExplorer(settings)) {
            | Ok(url) =>
              electron##shell##openExternal(url ++ operation.hash)->ignore
            | Error(err) =>
              addToast(Logs.error(~origin=Settings, Network.errorMsg(err)))
            }
          }}
        />
      </CellAction>
    </Table.Row>;
  });
