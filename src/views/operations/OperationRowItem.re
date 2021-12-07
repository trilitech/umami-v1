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
  let make = (~address: PublicKeyHash.t, ~operation: Operation.Read.t) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let tooltip = (
      "add_contact_from_op" ++ operation.hash ++ operation.op_id->string_of_int,
      I18n.tooltip#add_contact,
    );

    let onPress = _e => openAction();

    <>
      <IconButton icon=Icons.AddContact.build onPress tooltip />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ContactFormView initAddress=address action=Create closeAction />
      </ModalAction>
    </>;
  };
};

let rawUnknownAddress = (address: PublicKeyHash.t, operation) => {
  <View style=styles##rawAddressContainer>
    <Typography.Address numberOfLines=1>
      (address :> string)->React.string
    </Typography.Address>
    <AddContactButton address operation />
  </View>;
};

let getContactOrRaw = (aliases, tokens, address, operation) => {
  address
  ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
  ->Option.mapWithDefault(rawUnknownAddress(address, operation), alias =>
      <Typography.Body1 numberOfLines=1>
        alias->React.string
      </Typography.Body1>
    );
};

let status =
    (operation: Operation.Read.t, currentLevel, config: ConfigContext.env) => {
  let (txt, colorStyle) =
    switch (operation.status) {
    | Mempool => (I18n.t#state_mempool, Some(`negative))
    | Chain =>
      let minConfirmations = config.confirmations;
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

module AddToken = {
  [@react.component]
  let make = (~address, ~op: Operation.Read.t) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();
    let closeAction = () => {
      closeAction();
    };

    let apiVersion: option(Network.apiVersion) = StoreContext.useApiVersion();

    let chain = {
      apiVersion->Option.map(v => v.chain)->Option.getWithDefault("");
    };

    let tooltip = (
      "add_token_from_op" ++ op.hash ++ op.op_id->string_of_int,
      I18n.tooltip#add_token,
    );
    let onPress = _ => openAction();

    <>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <TokenAddView chain address=(address :> string) closeAction />
      </ModalAction>
      <IconButton
        icon=Icons.AddToken.build
        iconSizeRatio={5. /. 7.}
        onPress
        tooltip
      />
    </>;
  };
};

let amount =
    (
      account,
      transaction: Operation.Transaction.t,
      tokens,
      op: Operation.Read.t,
    ) => {
  let colorStyle =
    account->Option.map((account: Account.t) =>
      account.address
      == transaction->Operation.Transaction.Accessor.destination
        ? `positive : `negative
    );

  let sign = colorStyle == Some(`positive) ? "+" : "-";
  <CellAmount>
    {<Typography.Body1 ?colorStyle>
       {switch (transaction) {
        | Tez(transaction) =>
          I18n.t#tez_op_amount(sign, transaction.amount->Tez.toString)
          ->React.string
        | Token(_, token_trans) =>
          let address = token_trans.contract;
          let token: option((Token.t, ReBigNumber.t)) =
            TokensLibrary.WithBalance.getFullToken(tokens, address, 0);
          switch (token) {
          | None =>
            let tooltip = (
              "unknown_token" ++ op.hash ++ op.op_id->string_of_int,
              I18n.tooltip#unregistered_token_transaction,
            );
            <View style=styles##rawAddressContainer>
              <Text>
                {Format.asprintf(
                   "%s %s",
                   sign,
                   token_trans.amount->TokenRepr.Unit.toNatString,
                 )
                 ->React.string}
              </Text>
              <IconButton
                icon=Icons.QuestionMark.build
                size=19.
                iconSizeRatio=1.
                tooltip
                disabled=true
                style=Style.(style(~borderRadius=0., ~marginLeft="4px", ()))
              />
              <AddToken address=(address :> string) op />
            </View>;
          | Some(({symbol, decimals, _}, _)) =>
            Format.asprintf(
              "%s %s %s",
              sign,
              token_trans.amount->TokenRepr.Unit.toStringDecimals(decimals),
              symbol,
            )
            ->React.string
          };
        }}
     </Typography.Body1>}
  </CellAmount>;
};

[@react.component]
let make =
  memo((~operation: Operation.Read.t, ~currentLevel) => {
    let account = StoreContext.SelectedAccount.useGet();
    let aliases = StoreContext.Aliases.useGetAll();
    let tokens = StoreContext.Tokens.useGetAll();
    let config = ConfigContext.useContent();
    let addToast = LogsContext.useToast();

    <Table.Row>
      {switch (operation.payload) {
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
               {I18n.t#tez_amount(operation.fee->Tez.toString)->React.string}
             </Typography.Body1>
           </CellFee>
           <CellAddress />
           <CellAddress />
         </>
       | Transaction(Token(common, _) as transaction)
       | Transaction(Tez(common) as transaction) =>
         <>
           <CellType>
             <Typography.Body1>
               I18n.t#operation_transaction->React.string
             </Typography.Body1>
           </CellType>
           {amount(account, transaction, tokens, operation)}
           <CellFee>
             <Typography.Body1>
               {I18n.t#tez_amount(operation.fee->Tez.toString)->React.string}
             </Typography.Body1>
           </CellFee>
           <CellAddress>
             {operation.source
              ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
              ->Option.mapWithDefault(
                  rawUnknownAddress(operation.source, operation), alias =>
                  <Typography.Body1 numberOfLines=1>
                    alias->React.string
                  </Typography.Body1>
                )}
           </CellAddress>
           <CellAddress>
             {getContactOrRaw(aliases, tokens, common.destination, operation)}
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
               {I18n.t#tez_amount(operation.fee->Tez.toString)->React.string}
             </Typography.Body1>
           </CellFee>
           <CellAddress>
             {getContactOrRaw(aliases, tokens, operation.source, operation)}
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
                  {getContactOrRaw(aliases, tokens, d, operation)}
                </CellAddress>
              )}
         </>
       | Unknown =>
         <>
           <CellType>
             <Typography.Body1>
               I18n.t#unknown_operation->ReasonReact.string
             </Typography.Body1>
           </CellType>
           <CellAmount />
           <CellFee />
           <CellAddress />
         </>
       }}
      <CellDate>
        <Typography.Body1>
          {operation.timestamp->DateFns.format("P pp")->React.string}
        </Typography.Body1>
      </CellDate>
      <CellStatus> {status(operation, currentLevel, config)} </CellStatus>
      <CellAction>
        <IconButton
          size=34.
          icon=Icons.OpenExternal.build
          tooltip=(
            "open_in_explorer"
            ++ operation.hash
            ++ operation.op_id->string_of_int,
            I18n.tooltip#open_in_explorer,
          )
          onPress={_ => {
            switch (Network.externalExplorer(config.network.chain)) {
            | Ok(url) => System.openExternal(url ++ operation.hash)
            | Error(err) => addToast(Logs.error(~origin=Operation, err))
            }
          }}
        />
      </CellAction>
    </Table.Row>;
  });
