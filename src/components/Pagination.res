open ReactNative

module Icon = {
  @react.component
  let make = (~icon: Icons.builder, ~onPress=?, ~disabled=?, ~style=?) =>
    <IconButton icon iconSizeRatio=1. ?disabled ?onPress ?style />
}

let styles = {
  open Style
  StyleSheet.create({
    "container": style(~flex=1., ()),
    "footer": style(~marginHorizontal=LayoutConst.pagePaddingHorizontal->dp, ()),
    "listContent": style(
      ~flex=1.,
      ~paddingTop=4.->dp,
      ~paddingBottom=LayoutConst.pagePaddingVertical->dp,
      ~marginHorizontal=LayoutConst.pagePaddingHorizontal->dp,
      (),
    ),
    "marginLeft": style(~marginLeft="10px", ()),
    "marginRight": style(~marginRight="10px", ()),
    "takeSpaceAndCenter": style(~flexBasis="80px", ~textAlign=#center, ()),
  })
}

module PaginationFooter = {
  @react.component
  let make = (~pageCounter, ~setPageCounter, ~maxPageIndex, ~onPageChange=?, ~style=?) => {
    let onPageChange = onPageChange->Option.getWithDefault(_ => ())
    let theme = ThemeContext.useTheme()
    let pagination = Style.arrayOption([
      style,
      Some(
        Style.style(
          ~justifyContent=#center,
          ~flexDirection=#row,
          ~alignItems=#center,
          ~marginBottom="20px",
          ~borderTopWidth=1.,
          ~borderTopColor=theme.colors.borderDisabled,
          ~marginTop="20px",
          ~paddingTop="20px",
          (),
        ),
      ),
    ])

    let pageStr = Format.asprintf("%d / %d", pageCounter + 1, maxPageIndex + 1)

    <View style=pagination>
      <Icon
        icon=Icons.FirstPage.build
        disabled={pageCounter <= 0}
        onPress={_ => {
          onPageChange()
          setPageCounter(_ => 0)
        }}
        style={styles["marginRight"]}
      />
      <Icon
        icon=Icons.Previous.build
        disabled={pageCounter <= 0}
        onPress={_ => {
          onPageChange()
          setPageCounter(i => i - 1)
        }}
        style={styles["marginRight"]}
      />
      <Typography.Body1 style={styles["takeSpaceAndCenter"]}>
        {pageStr->ReasonReact.string}
      </Typography.Body1>
      <Icon
        icon=Icons.Next.build
        disabled={pageCounter >= maxPageIndex}
        onPress={_ => {
          onPageChange()
          setPageCounter(i => i + 1)
        }}
        style={styles["marginLeft"]}
      />
      <Icon
        icon=Icons.LastPage.build
        disabled={pageCounter >= maxPageIndex}
        onPress={_ => {
          onPageChange()
          setPageCounter(_ => maxPageIndex)
        }}
        style={styles["marginLeft"]}
      />
    </View>
  }
}

@react.component
let make = (~footerStyle=?, ~elements, ~renderItem, ~emptyComponent, ~eltsPerPage=20) =>
  switch elements->Array.length {
  | 0 => <View style={styles["listContent"]}> <Table.Empty> emptyComponent </Table.Empty> </View>
  | _ =>
    let scrollViewRef = React.useRef(Js.Nullable.null)
    let scrollToTop = (scrollRef: React.ref<_>) =>
      switch scrollRef.current->Js.Nullable.toOption {
      | None => ()
      | Some(scrollView) =>
        scrollView->{
          open ScrollView
          scrollTo(scrollToParams(~x=0., ~y=0., ~animated=false, ()))
        }
      }
    let maxPageIndex = (elements->Array.length - 1) / eltsPerPage
    let (pageCounter, setPageCounter) = React.useState(_ => 0)
    // We shadow page counter to disallow out-of-bounds changes.
    let setPageCounter = f =>
      setPageCounter(i => {
        let ret = f(i)
        if ret > maxPageIndex || ret < 0 {
          i
        } else {
          ret
        }
      })
    let data = elements->Array.slice(~offset=pageCounter * eltsPerPage, ~len=eltsPerPage)
    <>
      <ScrollView ref={scrollViewRef->Ref.value} style={styles["listContent"]}>
        {data->Array.map(renderItem)->ReasonReact.array}
      </ScrollView>
      <PaginationFooter
        pageCounter
        setPageCounter
        maxPageIndex
        onPageChange={() => scrollToTop(scrollViewRef)}
        style={Style.arrayOption([Some(styles["footer"]), footerStyle])}
      />
    </>
  }
