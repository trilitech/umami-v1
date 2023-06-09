import React from 'react';
import { View, StyleSheet } from "react-native";

const styles = StyleSheet.create({"view": {willChange: "transform"}});

const ViewWithFlippedProps = ({children, zIndex, flippedProps}) => {
  const dataSet = Object.fromEntries(
    Object.entries(flippedProps).map(([key, value]) => [key.replace("data-",""),  value])
  );
  return (<View style={[styles.view, { zIndex }]} dataSet={dataSet} >{children}</View>);
};

export default ViewWithFlippedProps;
