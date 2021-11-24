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

let styles =
  Style.(
    StyleSheet.create({
      "listContent":
        style(
          ~flex=1.,
          ~paddingTop=4.->dp,
          ~paddingBottom=LayoutConst.pagePaddingVertical->dp,
          ~paddingHorizontal=LayoutConst.pagePaddingHorizontal->dp,
          (),
        ),
    })
  );

type mode =
  | Gallery
  | Collection;

module Component = {
  [@react.component]
  let make = (~account) => {
    let (mode, setMode) = React.useState(_ => Gallery);
    let onRefresh = StoreContext.Tokens.useResetAllAccountsTokens();

    let tokensRequest =
      StoreContext.Tokens.useAccountsTokens(
        TokensApiRequest.{
          account: account.Account.address,
          index: 0,
          numberByAccount: 10,
        },
      );

    let tokens =
      React.useMemo1(
        () =>
          switch (tokensRequest) {
          | Done(Ok(tokens), _)
          | Loading(Some(tokens)) =>
            tokens.TokensApiRequest.sorted->Nft.fromCache
          | _ => []
          },
        [|tokensRequest|],
      );

    let (switchButtonText, icon) =
      switch (mode) {
      | Gallery => (I18n.btn#collected, Icons.Collection.build)
      | Collection => (I18n.btn#gallery, Icons.Gallery.build)
      };

    let headline =
      switch (mode) {
      | Gallery => I18n.title#gallery
      | Collection => I18n.title#collected
      };

    let loading = !tokensRequest->ApiRequest.isDone;

    <View style={styles##listContent}>
      <NftHeaderView headline>
        <RefreshButton onRefresh loading=false />
        <ButtonAction
          style={Style.style(~marginTop="10px", ())}
          icon
          text=switchButtonText
          onPress={_ =>
            setMode(
              fun
              | Gallery => Collection
              | Collection => Gallery,
            )
          }
        />
      </NftHeaderView>
      {switch (mode) {
       | Gallery => <NftGalleryView loading nfts={tokens->Nft.flatten} />
       | Collection => <NftCollectionView account nfts=tokens />
       }}
    </View>;
  };
};

[@react.component]
let make = () => {
  let account = StoreContext.SelectedAccount.useGet();

  switch (account) {
  | Some(account) => <Component account />
  | None => <OperationsView.Placeholder />
  };
};
