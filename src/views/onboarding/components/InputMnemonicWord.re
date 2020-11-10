open ReactNative;

let itemHeight = 26.;
let listVerticalPadding = 8.;
let numItemsToDisplay = 8.;

module Item = {
  let styles =
    Style.(
      StyleSheet.create({
        "itemContainer":
          style(
            ~height=itemHeight->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            ~paddingLeft=(10. +. 17. +. 13.)->dp,
            (),
          ),
        "itemContainerHovered":
          style(~backgroundColor=Theme.colorDarkSelected, ()),
      })
    );

  [@react.component]
  let make = (~value, ~index, ~isSelected, ~onChange, ~onSelect) => {
    let onPress = _event => {
      onChange(value);
    };
    let onMouseDown = event => {
      event->ReactEvent.Mouse.preventDefault;
    };
    let onMouseMove = _event => {
      onSelect(_ => index);
    };

    <View onMouseDown onMouseMove>
      <TouchableOpacity onPress>
        <View
          style=Style.(
            arrayOption([|
              Some(styles##itemContainer),
              isSelected ? Some(styles##itemContainerHovered) : None,
            |])
          )>
          <Typography.Body3 colorStyle=`mediumEmphasis>
            value->React.string
          </Typography.Body3>
        </View>
      </TouchableOpacity>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "input":
        style(
          ~paddingHorizontal=10.->dp,
          ~paddingLeft=(10. +. 17. +. 13.)->dp,
          ~height=36.->dp,
          ~borderColor=Theme.colorDarkMediumEmphasis,
          ~borderWidth=1.,
          ~borderRadius=4.,
          ~fontFamily="Avenir",
          ~color=Theme.colorDarkHighEmphasis,
          ~fontSize=16.,
          ~fontWeight=`normal,
          (),
        ),
      "inputError":
        style(
          ~color=Theme.colorDarkError,
          ~borderColor=Theme.colorDarkError,
          (),
        ),
      "wordItemIndexContainer":
        style(
          ~position=`absolute,
          ~left=10.->dp,
          ~top=0.->dp,
          ~bottom=0.->dp,
          ~justifyContent=`center,
          (),
        ),
      "wordItemIndex": style(~width=17.->dp, ~textAlign=`right, ()),
      "listContainer":
        style(
          ~position=`absolute,
          ~top=0.->dp,
          ~left=0.->dp,
          ~right=0.->dp,
          ~maxHeight=
            (itemHeight *. numItemsToDisplay +. listVerticalPadding *. 2.)->dp,
          ~paddingVertical=listVerticalPadding->dp,
          ~backgroundColor="#2e2e2e",
          ~borderRadius=3.,
          (),
        ),
    })
  );

[@react.component]
let make = (~verifyIndex, ~value, ~handleChange, ~error) => {
  let hasError = error->Belt.Option.isSome;
  let wordlists =
    Bip39.wordlistsEnglish
    ->Belt.Array.keep(Js.String.startsWith(value))
    ->Belt.Array.slice(~offset=0, ~len=12);

  let textInputRef = React.useRef(Js.Nullable.null);
  let scrollViewRef = React.useRef(Js.Nullable.null);

  let scrollYRef = React.useRef(0.);

  let (selectedWordIndex, setSelectedWordIndex) = React.useState(_ => 0);
  let (hasFocus, setHasFocus) = React.useState(_ => false);

  let displayError = hasError && !hasFocus;

  let onChangeItem = newValue => {
    handleChange(newValue);
    textInputRef.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(TextInput.blur)
    ->ignore;
  };

  let onKeyPress = keyPressEvent => {
    let key = keyPressEvent->TextInput.KeyPressEvent.nativeEvent##key;

    switch (key) {
    | "ArrowUp" =>
      let newIndex = Js.Math.max_int(0, selectedWordIndex - 1);
      setSelectedWordIndex(_ => newIndex);

      let topPosition = listVerticalPadding;
      let itemPosition = newIndex->float_of_int *. itemHeight;
      let itemScrollPosition = itemPosition -. topPosition;

      if (itemScrollPosition < scrollYRef.current) {
        scrollViewRef.current
        ->Js.Nullable.toOption
        ->Belt.Option.map(scrollElement =>
            scrollElement->ScrollView.scrollTo(
              ScrollView.scrollToParams(~x=0., ~y=itemScrollPosition, ()),
            )
          )
        ->ignore;
      };
    | "ArrowDown" =>
      let newIndex =
        Js.Math.min_int(
          wordlists->Belt.Array.size - 1,
          selectedWordIndex + 1,
        );
      setSelectedWordIndex(_ => newIndex);

      let topPosition = itemHeight *. (numItemsToDisplay -. 1.);
      let itemPosition = newIndex->float_of_int *. itemHeight;
      let itemScrollPosition = itemPosition -. topPosition;

      if (itemScrollPosition > scrollYRef.current) {
        scrollViewRef.current
        ->Js.Nullable.toOption
        ->Belt.Option.map(scrollElement =>
            scrollElement->ScrollView.scrollTo(
              ScrollView.scrollToParams(~x=0., ~y=itemScrollPosition, ()),
            )
          )
        ->ignore;
      };
    | "Enter" =>
      wordlists
      ->Belt.Array.get(selectedWordIndex)
      ->Belt.Option.map(onChangeItem)
      ->ignore
    | _ => ()
    };
  };

  let onScroll = scrollEvent => {
    scrollYRef.current =
      scrollEvent->Event.ScrollEvent.nativeEvent##contentOffset##y;
  };

  <View>
    <View style=styles##wordItemIndexContainer>
      <Typography.Subtitle1
        colorStyle={displayError ? `error : `mediumEmphasis}
        style=styles##wordItemIndex>
        {(verifyIndex + 1)->string_of_int->React.string}
      </Typography.Subtitle1>
    </View>
    <TextInput
      ref={textInputRef->Ref.value}
      style=Style.(
        arrayOption([|
          Some(styles##input),
          displayError ? Some(styles##inputError) : None,
        |])
      )
      value
      onChange={(event: TextInput.changeEvent) =>
        handleChange(event.nativeEvent.text)
      }
      onFocus={_ => setHasFocus(_ => true)}
      onBlur={_ => setHasFocus(_ => false)}
      onKeyPress
      autoCapitalize=`none
      autoCorrect=false
      autoFocus=false
    />
    {hasFocus && wordlists->Belt.Array.size > 0
       ? <View>
           <ScrollView
             ref={scrollViewRef->Ref.value}
             style=styles##listContainer
             onScroll
             scrollEventThrottle=16>
             {wordlists
              ->Belt.Array.mapWithIndex((index, word) =>
                  <Item
                    key=word
                    value=word
                    index
                    isSelected={index == selectedWordIndex}
                    onSelect=setSelectedWordIndex
                    onChange=onChangeItem
                  />
                )
              ->React.array}
           </ScrollView>
         </View>
       : React.null}
  </View>;
};
