open ReactNative;

module Item = {
  let styles =
    Style.(
      StyleSheet.create({
        "itemContainer":
          style(
            ~flexDirection=`row,
            //~alignItems=`center,
            (),
          ),
        "itemContainerHovered":
          style(~backgroundColor=Theme.colorDarkSelected, ()),
      })
    );

  [@react.component]
  let make =
      (
        ~value,
        ~index,
        ~isSelected,
        ~onChange,
        ~onSelect,
        ~itemHeight,
        ~children,
      ) => {
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
              Some(style(~height=itemHeight->dp, ())),
              isSelected ? Some(styles##itemContainerHovered) : None,
            |])
          )>
          children
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
          ~height=46.->dp,
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
      "listContainer":
        style(
          ~position=`absolute,
          ~top=0.->dp,
          ~left=0.->dp,
          ~right=0.->dp,
          ~backgroundColor="#2e2e2e",
          ~borderRadius=3.,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~value,
      ~handleChange: string => unit,
      ~error,
      ~list: array('item),
      ~renderItem: 'item => React.element,
      ~keyExtractor: 'item => string,
      ~renderLabel: option(bool => React.element)=?,
      ~style as styleFromProp=?,
      ~itemHeight=26.,
      ~listVerticalPadding=8.,
      ~numItemsToDisplay=8.,
    ) => {
  let hasError = error->Belt.Option.isSome;

  let textInputRef = React.useRef(Js.Nullable.null);
  let scrollViewRef = React.useRef(Js.Nullable.null);

  let scrollYRef = React.useRef(0.);

  let (selectedItemIndex, setSelectedItemIndex) = React.useState(_ => 0);
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
      let newIndex = Js.Math.max_int(0, selectedItemIndex - 1);
      setSelectedItemIndex(_ => newIndex);

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
        Js.Math.min_int(list->Belt.Array.size - 1, selectedItemIndex + 1);
      setSelectedItemIndex(_ => newIndex);

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
      list
      ->Belt.Array.get(selectedItemIndex)
      ->Belt.Option.map(item => onChangeItem(item->keyExtractor))
      ->ignore
    | _ => ()
    };
  };

  let onScroll = scrollEvent => {
    scrollYRef.current =
      scrollEvent->Event.ScrollEvent.nativeEvent##contentOffset##y;
  };

  <View>
    {renderLabel->Belt.Option.mapWithDefault(React.null, renderLabel =>
       renderLabel(displayError)
     )}
    <TextInput
      ref={textInputRef->Ref.value}
      style=Style.(
        arrayOption([|
          Some(styles##input),
          displayError ? Some(styles##inputError) : None,
          styleFromProp,
        |])
      )
      value
      onChange={(event: TextInput.changeEvent) => {
        handleChange(event.nativeEvent.text);
        setSelectedItemIndex(_ => 0);
      }}
      onFocus={_ => setHasFocus(_ => true)}
      onBlur={_ => {
        setHasFocus(_ => false);
        setSelectedItemIndex(_ => 0);
      }}
      onKeyPress
      autoCapitalize=`none
      autoCorrect=false
      autoFocus=false
    />
    {hasFocus && list->Belt.Array.size > 0 && value->Js.String.length > 0
       ? <View>
           <ScrollView
             ref={scrollViewRef->Ref.value}
             style=Style.(
               array([|
                 styles##listContainer,
                 style(
                   ~maxHeight=
                     (
                       itemHeight
                       *. numItemsToDisplay
                       +. listVerticalPadding
                       *. 2.
                     )
                     ->dp,
                   ~paddingVertical=listVerticalPadding->dp,
                   (),
                 ),
               |])
             )
             onScroll
             scrollEventThrottle=16>
             {list
              ->Belt.Array.mapWithIndex((index, item) =>
                  <Item
                    key={item->keyExtractor}
                    value={item->keyExtractor}
                    index
                    isSelected={index == selectedItemIndex}
                    itemHeight
                    onSelect=setSelectedItemIndex
                    onChange=onChangeItem>
                    {renderItem(item)}
                  </Item>
                )
              ->React.array}
           </ScrollView>
         </View>
       : React.null}
  </View>;
};
