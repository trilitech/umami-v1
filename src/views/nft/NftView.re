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
      "searchSection":
        Style.(
          style(
            ~maxHeight=44.->dp,
            ~flexBasis=48.->dp,
            ~flex=1.,
            ~flexDirection=`row,
            ~alignItems=`center,
            (),
          )
        ),
      "listContent":
        style(
          ~flex=1.,
          ~paddingTop=4.->dp,
          ~paddingBottom=LayoutConst.pagePaddingVertical->dp,
          ~paddingHorizontal=LayoutConst.pagePaddingHorizontal->dp,
          (),
        ),
      "searchAndSync": style(~flexDirection=`row, ()),
    })
  );

type mode =
  | Gallery
  | Collection;

module Component = {
  [@react.component]
  let make = (~account) => {
    let (mode, setMode) = React.useState(_ => Gallery);
    let (syncState, setSyncState) = React.useState(_ => Sync.NotInitiated);
    let (search, setSearch) = React.useState(_ => "");
    let stop = React.useRef(false);

    let request = fromCache =>
      TokensApiRequest.{
        request:
          NFT.{
            account: account.Account.address,
            allowHidden: true,
            numberByAccount: BCD.requestPageSize,
          },
        fromCache,
      };

    // will be used to indicate a percentage of NFTs loaded
    let onTokens = (~total, ~lastToken) => {
      let percentage =
        Int.toFloat(lastToken + 1) /. Int.toFloat(total) *. 100.;
      setSyncState(
        fun
        | Canceled(_) => Canceled(percentage)
        | _ => Loading(percentage),
      );
    };
    let onStop = () => stop.current;

    let (tokensRequest, getTokens) =
      StoreContext.Tokens.useAccountNFTs(
        onTokens,
        onStop,
        account.Account.address,
        request(true),
      );

    let (switchButtonText, icon) =
      switch (mode) {
      | Gallery => (I18n.Btn.collected, Icons.Collection.build)
      | Collection => (I18n.Btn.gallery, Icons.Gallery.build)
      };

    let headline =
      switch (mode) {
      | Gallery => I18n.Title.gallery
      | Collection => I18n.Title.collected
      };

    let tokens =
      switch (tokensRequest) {
      | Done(Ok(`Cached(tokens) | `Fetched(tokens, _)), _)
      | Loading(Some(`Cached(tokens) | `Fetched(tokens, _))) => tokens
      | _ => PublicKeyHash.Map.empty
      };

    let loadToCanceled = () =>
      setSyncState(
        fun
        | Loading(percentage) => Canceled(percentage)
        | _ => NotInitiated,
      );
    let loadToDone = () =>
      setSyncState(
        fun
        | Loading(_) => Done
        | state => state,
      );

    React.useEffect1(
      () =>
        switch (tokensRequest) {
        | Done(Ok(`Fetched(_, _)), _) =>
          loadToDone();
          stop.current = false;
          None;

        | Done(Ok(`Cached(_)), _) =>
          setSyncState(_ => NotInitiated);
          None;

        | Done(Error(_), _) =>
          loadToCanceled();
          None;

        | _ => None
        },
      [|tokensRequest|],
    );

    let tokens =
      React.useMemo2(
        () => {
          let searched = search->Js.String.toLocaleLowerCase;
          tokens->TokensLibrary.Generic.keepTokens((_, _, (token, _)) =>
            token
            ->TokensLibrary.Token.name
            ->Option.mapWithDefault(false, name =>
                name
                ->Js.String.toLocaleLowerCase
                ->Js.String2.includes(searched)
              )
          );
        },
        (search, tokens),
      );

    let onRefresh = () => {
      setSyncState(_ => Loading(0.));
      getTokens(request(false))->ignore;
    };

    let onStop = () => {
      setSyncState(
        fun
        | Loading(percentage) => {
            Canceled(percentage);
          }
        | state => state,
      );
      stop.current = true;
    };

    let nfts =
      React.useMemo1(
        () =>
          TokensLibrary.(
            tokens->Generic.keepTokens((_, _, (t, _)) =>
              switch (t) {
              | Token.Full(_) => true
              | _ => false
              }
            )
          ),
        [|tokens|],
      );

    <View style={styles##listContent}>
      <NftHeaderView headline>
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
      <View style={styles##searchAndSync}>
        <ThemedTextInput
          style=styles##searchSection
          icon=Icons.Search.build
          value=search
          onValueChange={value => setSearch(_ => value)}
          placeholder=I18n.Input_placeholder.search_for_nft
        />
        <Sync onRefresh onStop state=syncState />
      </View>
      {switch (mode) {
       | Gallery => <NftGalleryView nfts />
       | Collection => <NftCollectionView account nfts />
       }}
    </View>;
  };
};

[@react.component]
let make = () => {
  let account = StoreContext.SelectedAccount.useGet();

  switch (account) {
  | Some(account) => <Component account />
  | None => <NftEmptyView />
  };
};
