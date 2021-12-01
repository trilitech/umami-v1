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

module Item = {
  let styles =
    Style.(
      StyleSheet.create({"itemContainer": style(~flexDirection=`row, ())})
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

let styles =
  Style.(
    StyleSheet.create({
      "menuCompensateInputPadding":
        style(
          ~marginTop=8.->dp,
          ~marginRight=(-12.)->dp,
          ~marginLeft=(-21.)->dp,
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
      ~placeholder=?,
      ~clearButton=false,
      ~reversePositionPct=?,
      ~keyPopover,
      ~dropdownOnEmpty=true,
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

  let dropdownReversed = React.useRef(false);

  let (selectedItemIndex, setSelectedItemIndex) = React.useState(_ => 0);
  let (hasFocus, setHasFocus) = React.useState(_ => false);
  let (popoverConfig, setPopoverConfig) = React.useState(_ => None);

  let displayError = hasError && !hasFocus;

  let onChangeItem = newValue => {
    handleChange(newValue);
    textInputRef.current->Js.Nullable.toOption->Option.iter(TextInput.blur);
  };

  let onKeyPress = keyPressEvent => {
    let key = keyPressEvent->TextInput.KeyPressEvent.nativeEvent##key;

    switch (key, dropdownReversed.current) {
    | ("ArrowUp", false)
    | ("ArrowDown", true) =>
      let newIndex = Js.Math.max_int(0, selectedItemIndex - 1);
      setSelectedItemIndex(_ => newIndex);

      let topPosition = DropdownMenu.listVerticalPadding;
      let itemPosition = newIndex->float_of_int *. itemHeight;
      let itemScrollPosition = itemPosition -. topPosition;

      if (itemScrollPosition < scrollYRef.current) {
        scrollViewRef.current
        ->Js.Nullable.toOption
        ->Option.iter(scrollElement =>
            scrollElement->ScrollView.scrollTo(
              ScrollView.scrollToParams(~x=0., ~y=itemScrollPosition, ()),
            )
          );
      };
    | ("ArrowDown", false)
    | ("ArrowUp", true) =>
      let newIndex =
        Js.Math.min_int(list->Array.size - 1, selectedItemIndex + 1);
      setSelectedItemIndex(_ => newIndex);

      let topPosition = itemHeight *. (numItemsToDisplay -. 1.);
      let itemPosition = newIndex->float_of_int *. itemHeight;
      let itemScrollPosition = itemPosition -. topPosition;

      if (itemScrollPosition > scrollYRef.current) {
        scrollViewRef.current
        ->Js.Nullable.toOption
        ->Option.iter(scrollElement =>
            scrollElement->ScrollView.scrollTo(
              ScrollView.scrollToParams(~x=0., ~y=itemScrollPosition, ()),
            )
          );
      };
    | ("Enter", _)
    | ("Tab", _) when value != "" =>
      list
      ->Array.get(selectedItemIndex)
      ->Option.iter(item => onChangeItem(item->keyExtractor))
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
      onClear=?{clearButton ? Some(() => onChangeItem("")) : None}
      paddingVertical=?inputPaddingVertical
      ?placeholder
      value
      hasError
      onValueChange={newValue => {
        handleChange(newValue);
        setSelectedItemIndex(_ => 0);
      }}
      onFocus={_ => {
        textInputRef.current
        ->Js.Nullable.toOption
        ->Option.iter(textInputElement => {
            textInputElement->ThemedTextInput.measureInWindow(
              (~x, ~y, ~width, ~height) => {
              setPopoverConfig(_ => Some(Popover.{x, y, width, height}))
            })
          });
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
      ?reversePositionPct
      onScroll
      scrollEventThrottle=16
      popoverConfig
      isOpen={
        hasFocus && list->Array.size > 0 && (dropdownOnEmpty || value != "")
      }
      onRequestClose={_ => {
        setHasFocus(_ => false);
        setSelectedItemIndex(_ => 0);
      }}
      style=Style.(
        array([|
          styles##menuCompensateInputPadding,
          style(~backgroundColor=theme.colors.background, ()),
        |])
      )>
      {reversed => {
         dropdownReversed.current = reversed;
         list->Array.mapWithIndex((index, item) =>
           <Item
             key={item->keyExtractor}
             value={item->keyExtractor}
             index
             isSelected={
               /* reversed */
               /*     ? list->Array.length - selectedItemIndex - 1 == index */
               /* : */
               index == selectedItemIndex
             }
             itemHeight
             onSelect=setSelectedItemIndex
             onChange=onChangeItem>
             {renderItem(item)}
           </Item>
         );
       }}
    </DropdownMenu>
  </View>;
};
