import React from 'react';
import { View } from "react-native";

const FlippedView = ({children, flippedProps}) => {
  const dataSet = Object.fromEntries(
    Object.entries(flippedProps).map(([key, value]) => [key.replace("data-",""),  value])
  );
  return (<View dataSet={dataSet} >{children}</View>);
};

export default FlippedView;
