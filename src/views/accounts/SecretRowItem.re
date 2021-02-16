open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "inner": style(~marginLeft=14.->dp, ()),
      "alias": style(~height=20.->dp, ~marginBottom=4.->dp, ()),
      "derivation": style(~height=18.->dp, ()),
    })
  );

module AccountNestedRowItem = {
  [@react.component]
  let make = (~address: string, ~index: int, ~zIndex, ~isLast=false) => {
    let account = StoreContext.Accounts.useGetFromAddress(address);

    account->ReactUtils.mapOpt(account =>
      <RowItem.Bordered
        height=90. style={Style.style(~zIndex, ())} isNested=true isLast>
        <View style=styles##inner>
          <Typography.Subtitle1 style=styles##alias>
            account.alias->React.string
          </Typography.Subtitle1>
          <AccountInfoBalance address={account.address} />
          <Typography.Address style=styles##derivation>
            {("/" ++ index->string_of_int)->React.string}
          </Typography.Address>
        </View>
      </RowItem.Bordered>
    );
  };
};

module AccountImportedRowItem = {
  [@react.component]
  let make = (~address: string, ~zIndex) => {
    let account = StoreContext.Accounts.useGetFromAddress(address);

    account->ReactUtils.mapOpt(account =>
      <RowItem.Bordered height=66. style={Style.style(~zIndex, ())}>
        <View style=styles##inner>
          <Typography.Subtitle1 style=styles##alias>
            account.alias->React.string
          </Typography.Subtitle1>
          <AccountInfoBalance address={account.address} />
        </View>
      </RowItem.Bordered>
    );
  };
};

[@react.component]
let make = (~secret: AccountApiRequest.AccountsAPI.Secret.t, ~zIndex) => {
  <View style={Style.style(~zIndex, ())}>
    <RowItem.Bordered
      height=66.
      style={Style.style(~zIndex=secret.addresses->Array.size + 1, ())}>
      <View style=styles##inner>
        <Typography.Subtitle1 style=styles##alias>
          secret.name->React.string
        </Typography.Subtitle1>
        <Typography.Address style=styles##derivation>
          secret.derivationScheme->React.string
        </Typography.Address>
      </View>
    </RowItem.Bordered>
    {secret.addresses
     ->Array.mapWithIndex((index, address) =>
         <AccountNestedRowItem
           key=address
           address
           index
           zIndex={secret.addresses->Array.size - index}
           isLast={secret.addresses->Array.size - 1 === index}
         />
       )
     ->React.array}
  </View>;
};
