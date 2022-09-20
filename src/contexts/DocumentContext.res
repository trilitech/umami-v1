/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReactNative

type element = React.ref<Js.Nullable.t<NativeElement.element>>
type pressCallback = Event.pressEvent => unit
type scrollCallback = Event.scrollEvent => unit
type state = {
  addPressEventListener: (element, bool, pressCallback) => unit,
  removePressEventListener: (element, bool, pressCallback) => unit,
  scrollListeners: array<scrollCallback>,
  addScrollEventListener: scrollCallback => unit,
  removeScrollEventListener: scrollCallback => unit,
}

// Context and Provider

let initialState = {
  addPressEventListener: (_, _, _) => (),
  removePressEventListener: (_, _, _) => (),
  scrollListeners: [],
  addScrollEventListener: _ => (),
  removeScrollEventListener: _ => (),
}

let context = React.createContext(initialState)

module Provider = {
  let makeProps = (~value, ~children, ()) =>
    {
      "value": value,
      "children": children,
    }

  let make = React.Context.provider(context)
}

// Final Provider

let styles = {
  open Style
  StyleSheet.create({"document": style(~flex=1., ())})
}

@react.component
let make = (~children) => {
  let (clickOutsideListeners, setClickOutsideListeners) = React.useState(_ => [])

  let addPressEventListener = React.useMemo1(
    ((), element, isOpen, callback) =>
      setClickOutsideListeners(clickOutsideListeners =>
        clickOutsideListeners->Array.concat([(element, isOpen, callback)])
      ),
    [setClickOutsideListeners],
  )

  let removePressEventListener = React.useMemo1(
    ((), _element, _isOpen, callback) =>
      setClickOutsideListeners(clickOutsideListeners =>
        clickOutsideListeners->Array.keep(((_, _, cb)) => cb !== callback)
      ),
    [setClickOutsideListeners],
  )

  let (scrollListeners, setScrollListeners) = React.useState(_ => [])

  let addScrollEventListener = React.useMemo1(
    ((), callback) =>
      setScrollListeners(scrollListeners => scrollListeners->Array.concat([callback])),
    [setScrollListeners],
  )

  let removeScrollEventListener = React.useMemo1(
    ((), callback) =>
      setScrollListeners(scrollListeners => scrollListeners->Array.keep(cb => cb !== callback)),
    [setScrollListeners],
  )

  let value = React.useMemo5(() => {
    addPressEventListener: addPressEventListener,
    removePressEventListener: removePressEventListener,
    scrollListeners: scrollListeners,
    addScrollEventListener: addScrollEventListener,
    removeScrollEventListener: removeScrollEventListener,
  }, (
    addPressEventListener,
    removePressEventListener,
    scrollListeners,
    addScrollEventListener,
    removeScrollEventListener,
  ))

  let onStartShouldSetResponderCapture = React.useCallback1(
    pressEvent =>
      clickOutsideListeners->Array.some(((element, isOpen, _)) =>
        isOpen && element.current !== (pressEvent->Event.PressEvent.nativeEvent)["target"]
      ),
    [clickOutsideListeners],
  )

  let onResponderRelease = React.useCallback1(
    pressEvent =>
      clickOutsideListeners->Array.forEach(((_element, _isOpen, callback)) => callback(pressEvent)),
    [clickOutsideListeners],
  )

  <Portal.Provider>
    <View style={styles["document"]} onStartShouldSetResponderCapture onResponderRelease>
      <Provider value> children </Provider>
    </View>
  </Portal.Provider>
}

// Hooks

let useDocumentContext = () => React.useContext(context)

let useClickOutside = (element, isOpen, callback) => {
  let document = useDocumentContext()

  React.useEffect4(() => {
    document.addPressEventListener(element, isOpen, callback)
    Some(() => document.removePressEventListener(element, isOpen, callback))
  }, (document.addPressEventListener, document.removePressEventListener, isOpen, callback))
}

let useScrollListener = () => {
  let {scrollListeners} = useDocumentContext()

  let onScroll = React.useCallback1(
    scrollEvent => scrollListeners->Array.forEach(callback => callback(scrollEvent)),
    [scrollListeners],
  )

  onScroll
}

let useScroll = callback => {
  let document = useDocumentContext()

  React.useEffect3(() => {
    document.addScrollEventListener(callback)
    Some(() => document.removeScrollEventListener(callback))
  }, (document.addScrollEventListener, document.removeScrollEventListener, callback))
}

// Components

module ScrollView = {
  @react.component
  let make = (
    ~style=?,
    ~showsVerticalScrollIndicator=false,
    ~contentContainerStyle=?,
    ~alwaysBounceVertical=?,
    ~children,
  ) => {
    let onScroll = useScrollListener()
    <ScrollView
      showsVerticalScrollIndicator
      ?style
      ?contentContainerStyle
      ?alwaysBounceVertical
      onScroll
      scrollEventThrottle=250>
      children
    </ScrollView>
  }
}
