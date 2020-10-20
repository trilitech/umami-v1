open ReactNative;

module OperationItem = {
  let baseCellStyle = Style.(style(~flexShrink=0., ~marginRight=24.->dp, ()));
  let styles =
    Style.(
      StyleSheet.create({
        "container":
          style(
            ~height=48.->dp,
            ~paddingVertical=6.->dp,
            ~flexDirection=`row,
            (),
          ),
        "inner":
          style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
        "border":
          style(
            ~backgroundColor=Colors.border,
            ~width=4.->dp,
            ~marginRight=11.->dp,
            ~borderTopRightRadius=4.,
            ~borderBottomRightRadius=4.,
            (),
          ),
        "cellType":
          StyleSheet.flatten([|
            baseCellStyle,
            style(~flexBasis=90.->dp, ()),
          |]),
        "cellAmount":
          StyleSheet.flatten([|
            baseCellStyle,
            style(~flexBasis=120.->dp, ()),
          |]),
        "cellFee":
          StyleSheet.flatten([|
            baseCellStyle,
            style(~flexBasis=120.->dp, ()),
          |]),
        "cellAddress":
          StyleSheet.flatten([|
            baseCellStyle,
            style(~flexBasis=180.->dp, ~flexShrink=1., ~flexGrow=1., ()),
          |]),
        "cellDate":
          StyleSheet.flatten([|
            baseCellStyle,
            style(~flexBasis=220.->dp, ()),
          |]),
        "text":
          style(~color=Colors.stdText, ~fontSize=16., ~fontWeight=`_400, ()),
      })
    );

  let memo = component =>
    React.memoCustomCompareProps(component, (prevPros, nextProps) =>
      prevPros##operation == nextProps##operation
    );

  [@react.component]
  let make =
    memo((~operation: Operation.t) => {
      <View style=styles##container>
        <View style=styles##border />
        <View style=styles##inner>
          {switch (operation.payload) {
           | Business(business) =>
             switch (business.payload) {
             | Reveal(_reveal) =>
               <>
                 <View style=styles##cellType>
                   <Text style=styles##text> "Reveal"->React.string </Text>
                 </View>
                 <View style=styles##cellAmount />
                 <View style=styles##cellFee>
                   <Text style=styles##text> business.fee->React.string </Text>
                 </View>
                 <View style=styles##cellAddress />
                 <View style=styles##cellAddress />
               </>
             | Transaction(transaction) =>
               <>
                 <View style=styles##cellType>
                   <Text style=styles##text>
                     "Transaction"->React.string
                   </Text>
                 </View>
                 <View style=styles##cellAmount>
                   <Text style=styles##text>
                     transaction.amount->React.string
                   </Text>
                 </View>
                 <View style=styles##cellFee>
                   <Text style=styles##text> business.fee->React.string </Text>
                 </View>
                 <View style=styles##cellAddress>
                   <Text style=styles##text numberOfLines=1>
                     business.source->React.string
                   </Text>
                 </View>
                 <View style=styles##cellAddress>
                   <Text style=styles##text numberOfLines=1>
                     transaction.destination->React.string
                   </Text>
                 </View>
               </>
             | Origination(_origination) =>
               <>
                 <View style=styles##cellType>
                   <Text style=styles##text>
                     "Origination"->React.string
                   </Text>
                 </View>
                 <View style=styles##cellAmount />
                 <View style=styles##cellFee />
                 <View style=styles##cellAddress />
                 <View style=styles##cellAddress />
               </>
             | Delegation(_delegation) =>
               <>
                 <View style=styles##cellType>
                   <Text style=styles##text> "Delegation"->React.string </Text>
                 </View>
                 <View style=styles##cellAmount />
                 <View style=styles##cellFee />
                 <View style=styles##cellAddress />
                 <View style=styles##cellAddress />
               </>
             | Unknown => React.null
             }
           }}
          <View style=styles##cellDate>
            <Text style=styles##text>
              {operation.timestamp->Js.Date.toISOString->React.string}
            </Text>
          </View>
        </View>
      </View>
    });
};

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flex=1., ()),
      "header":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~height=30.->dp,
          ~marginBottom=4.->dp,
          ~marginHorizontal=Theme.pagePaddingHorizontal->dp,
          ~marginTop=Theme.pagePaddingVertical->dp,
          ~paddingLeft=15.->dp,
          ~borderColor="rgba(255,255,255,0.38)",
          ~borderBottomWidth=1.,
          (),
        ),
      "headerText":
        style(
          ~color="rgba(255,255,255, 0.8)",
          ~fontSize=14.,
          ~fontWeight=`_300,
          (),
        ),
      "list": style(~flex=1., ()),
      "listContent":
        style(
          ~paddingBottom=Theme.pagePaddingVertical->dp,
          ~paddingHorizontal=Theme.pagePaddingHorizontal->dp,
          (),
        ),
    })
  );

let renderItem =
    (renderItemProps: VirtualizedList.renderItemProps(Operation.t)) => {
  let operation = renderItemProps.item;
  <OperationItem operation />;
};

let keyExtractor = (operation: Operation.t, _i) => {
  operation.id;
};

[@react.component]
let make = () => {
  let operationsRequest =
    OperationApiRequest.useGetOperations(~limit=100, ());

  <View style=styles##container>
    <View style=styles##header>
      <View style=OperationItem.styles##cellType>
        <Text style=styles##headerText> "TYPE"->React.string </Text>
      </View>
      <View style=OperationItem.styles##cellAmount>
        <Text style=styles##headerText> "AMOUNT"->React.string </Text>
      </View>
      <View style=OperationItem.styles##cellFee>
        <Text style=styles##headerText> "FEE"->React.string </Text>
      </View>
      <View style=OperationItem.styles##cellAddress>
        <Text style=styles##headerText> "SENDER"->React.string </Text>
      </View>
      <View style=OperationItem.styles##cellAddress>
        <Text style=styles##headerText> "RECIPIENT"->React.string </Text>
      </View>
      <View style=OperationItem.styles##cellDate>
        <Text style=styles##headerText> "TIMESTAMP"->React.string </Text>
      </View>
    </View>
    {switch (operationsRequest) {
     | Done(Ok(operations)) =>
       <FlatList
         style=styles##list
         contentContainerStyle=styles##listContent
         data=operations
         initialNumToRender=20
         keyExtractor
         renderItem
       />
     | Done(Error(error)) => error->React.string
     | NotAsked
     | Loading => <LoadingView />
     }}
  </View>;
};
