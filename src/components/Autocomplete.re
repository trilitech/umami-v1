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

    let theme = ThemeContext.useTheme();

    <View onMouseDown onMouseMove>
      <TouchableOpacity onPress>
        <View
          style=Style.(
            arrayOption([|
              Some(styles##itemContainer),
              Some(style(~height=itemHeight->dp, ())),
              isSelected
                ? Some(
                    Style.style(
                      ~backgroundColor=theme.colors.stateHovered,
                      (),
                    ),
                  )
                : None,
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
      "listContainer":
        style(
          ~position=`absolute,
          ~top=0.->dp,
          ~left=0.->dp,
          ~right=0.->dp,
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

  let theme = ThemeContext.useTheme();

  <View>
    {renderLabel->Belt.Option.mapWithDefault(React.null, renderLabel =>
       renderLabel(displayError)
     )}
    <ThemedTextInput
      inputRef={textInputRef->Ref.value}
      style=?styleFromProp
      value
      hasError
      onValueChange={newValue => {
        handleChange(newValue);
        setSelectedItemIndex(_ => 0);
      }}
      onFocus={_ => setHasFocus(_ => true)}
      onBlur={_ => {
        setHasFocus(_ => false);
        setSelectedItemIndex(_ => 0);
      }}
      onKeyPress
    />
    {hasFocus && list->Belt.Array.size > 0 && value->Js.String.length > 0
       ? <View>
           <ScrollView
             ref={scrollViewRef->Ref.value}
             style=Style.(
               array([|
                 styles##listContainer,
                 style(
                   ~backgroundColor=theme.colors.background,
                   ~maxHeight=
                     (
                       itemHeight
                       *. numItemsToDisplay
                       +. listVerticalPadding
                       *. 2.
                     )
                     ->dp,
                   (),
                 ),
               |])
             )
             contentContainerStyle=Style.(
               style(
                 ~paddingVertical=listVerticalPadding->dp,
                 ~backgroundColor=theme.colors.stateActive,
                 (),
               )
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
