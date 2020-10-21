open ReactNative;

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
      "inner": style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
      "border":
        style(
          ~backgroundColor="#8D9093",
          ~width=4.->dp,
          ~marginRight=11.->dp,
          ~borderTopRightRadius=4.,
          ~borderBottomRightRadius=4.,
          (),
        ),
      "cellType":
        StyleSheet.flatten([|baseCellStyle, style(~flexBasis=90.->dp, ())|]),
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
                 <Typography.Body1> "Reveal"->React.string </Typography.Body1>
               </View>
               <View style=styles##cellAmount />
               <View style=styles##cellFee>
                 <Typography.Body1>
                   business.fee->React.string
                 </Typography.Body1>
               </View>
               <View style=styles##cellAddress />
               <View style=styles##cellAddress />
             </>
           | Transaction(transaction) =>
             <>
               <View style=styles##cellType>
                 <Typography.Body1>
                   "Transaction"->React.string
                 </Typography.Body1>
               </View>
               <View style=styles##cellAmount>
                 <Typography.Body1>
                   transaction.amount->React.string
                 </Typography.Body1>
               </View>
               <View style=styles##cellFee>
                 <Typography.Body1>
                   business.fee->React.string
                 </Typography.Body1>
               </View>
               <View style=styles##cellAddress>
                 <Typography.Body1 numberOfLines=1>
                   business.source->React.string
                 </Typography.Body1>
               </View>
               <View style=styles##cellAddress>
                 <Typography.Body1 numberOfLines=1>
                   transaction.destination->React.string
                 </Typography.Body1>
               </View>
             </>
           | Origination(_origination) =>
             <>
               <View style=styles##cellType>
                 <Typography.Body1>
                   "Origination"->React.string
                 </Typography.Body1>
               </View>
               <View style=styles##cellAmount />
               <View style=styles##cellFee />
               <View style=styles##cellAddress />
               <View style=styles##cellAddress />
             </>
           | Delegation(_delegation) =>
             <>
               <View style=styles##cellType>
                 <Typography.Body1>
                   "Delegation"->React.string
                 </Typography.Body1>
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
          <Typography.Body1>
            {operation.timestamp->Js.Date.toISOString->React.string}
          </Typography.Body1>
        </View>
      </View>
    </View>
  });
