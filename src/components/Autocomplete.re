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
                      ~backgroundColor=theme.colors.stateActive,
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
      ~keyPopover,
      ~style as styleFromProp=?,
      ~inputPaddingLeft=?,
      ~inputPaddingRight=?,
      ~inputPaddingVertical=?,
      ~itemHeight=26.,
      ~numItemsToDisplay=8.,
    ) => {
  let hasError = error->Option.isSome;

  let textInputRef = React.useRef(Js.Nullable.null);
  let scrollViewRef = React.useRef(Js.Nullable.null);

  let scrollYRef = React.useRef(0.);

  let (selectedItemIndex, setSelectedItemIndex) = React.useState(_ => 0);
  let (hasFocus, setHasFocus) = React.useState(_ => false);
  let (popoverConfig, setPopoverConfig) = React.useState(_ => None);

  let displayError = hasError && !hasFocus;

  let onChangeItem = newValue => {
    handleChange(newValue);
    textInputRef.current
    ->Js.Nullable.toOption
    ->Option.map(TextInput.blur)
    ->ignore;
  };

  let onKeyPress = keyPressEvent => {
    let key = keyPressEvent->TextInput.KeyPressEvent.nativeEvent##key;

    switch (key) {
    | "ArrowUp" =>
      let newIndex = Js.Math.max_int(0, selectedItemIndex - 1);
      setSelectedItemIndex(_ => newIndex);

      let topPosition = DropdownMenu.listVerticalPadding;
      let itemPosition = newIndex->float_of_int *. itemHeight;
      let itemScrollPosition = itemPosition -. topPosition;

      if (itemScrollPosition < scrollYRef.current) {
        scrollViewRef.current
        ->Js.Nullable.toOption
        ->Option.map(scrollElement =>
            scrollElement->ScrollView.scrollTo(
              ScrollView.scrollToParams(~x=0., ~y=itemScrollPosition, ()),
            )
          )
        ->ignore;
      };
    | "ArrowDown" =>
      let newIndex =
        Js.Math.min_int(list->Array.size - 1, selectedItemIndex + 1);
      setSelectedItemIndex(_ => newIndex);

      let topPosition = itemHeight *. (numItemsToDisplay -. 1.);
      let itemPosition = newIndex->float_of_int *. itemHeight;
      let itemScrollPosition = itemPosition -. topPosition;

      if (itemScrollPosition > scrollYRef.current) {
        scrollViewRef.current
        ->Js.Nullable.toOption
        ->Option.map(scrollElement =>
            scrollElement->ScrollView.scrollTo(
              ScrollView.scrollToParams(~x=0., ~y=itemScrollPosition, ()),
            )
          )
        ->ignore;
      };
    | "Enter" =>
      list
      ->Array.get(selectedItemIndex)
      ->Option.map(item => onChangeItem(item->keyExtractor))
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
    {renderLabel->Option.mapWithDefault(React.null, renderLabel =>
       renderLabel(displayError)
     )}
    <ThemedTextInput
      inputRef={textInputRef->Ref.value}
      style=?styleFromProp
      paddingLeft=?inputPaddingLeft
      paddingRight=?inputPaddingRight
      paddingVertical=?inputPaddingVertical
      value
      hasError
      onValueChange={newValue => {
        handleChange(newValue);
        setSelectedItemIndex(_ => 0);
      }}
      onFocus={_ => {
        textInputRef.current
        ->Js.Nullable.toOption
        ->Option.map(textInputElement => {
            textInputElement->ThemedTextInput.measureInWindow(
              (~x, ~y, ~width, ~height) => {
              setPopoverConfig(_ => Some(Popover.{x, y, width, height}))
            })
          })
        ->ignore;
        setHasFocus(_ => true);
      }}
      onBlur={_ => {
        setHasFocus(_ => false);
        setSelectedItemIndex(_ => 0);
      }}
      onKeyPress
    />
    <DropdownMenu
      keyPopover
      scrollRef={scrollViewRef->Ref.value}
      onScroll
      scrollEventThrottle=16
      isOpen={hasFocus && list->Array.size > 0 && value->Js.String.length > 0}
      popoverConfig
      onRequestClose={_ => {
        setHasFocus(_ => false);
        setSelectedItemIndex(_ => 0);
      }}
      style=Style.(
        array([|
          style(
            ~backgroundColor=theme.colors.background,
            ~maxHeight=
              (
                itemHeight
                *. numItemsToDisplay
                +. DropdownMenu.listVerticalPadding
                *. 2.
              )
              ->dp,
            (),
          ),
        |])
      )>
      {list
       ->Array.mapWithIndex((index, item) =>
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
    </DropdownMenu>
  </View>;
};
