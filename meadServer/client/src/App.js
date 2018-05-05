import React, { Component } from 'react';
import {LineChart} from 'react-easy-chart';
import Select from 'react-select';
import './App.css';
import 'react-select/dist/react-select.css';

class App extends Component
{
  state =
  {
    batchNames: [],
    batchData: [],
    selected: ''
  }

  // Fetch names after first mount
  componentDidMount()
  {
    this.getNames();
  }

  getNames = () =>
  {
    // Get the names and store them in state
    fetch('/api/batchNames')
      .then(res => res.json())
      .then(batchNames =>
      {
        this.setState({ batchNames: batchNames });
        if (batchNames.length > 0)
        {
          this.setState({ selected: batchNames[0]});
          this.selected = batchNames[0];
          this.getData();
        }
      });
  }

  getData = () =>
  {
    if (this.selected !== '')
    {
      fetch('/api/batchData/' + this.selected)
        .then(res => res.json())
        .then(data => this.setState({ batchData: data }));
    }
  }

  printCSV = () =>
  {
    if (this.selected !== '')
    {
      const Json2csvParser = require('json2csv').Parser;
      const { batchData } = this.state;

      var csvData = [];
      for (var i = 0; i < batchData.length; i++)
      {
        csvData.push({'Sample Time': batchData[i].x, 'temperature (F)': batchData[i].y});
      }

      try
      {
        const parser = new Json2csvParser(csvData);
        const csv = parser.parse(csvData);

        let csvContent = "data:text/csv;charset=utf-8,";
        csvContent += csv;

        var encodedUri = encodeURI(csvContent);

        var link = document.createElement("a");
        link.setAttribute("href", encodedUri);
        link.setAttribute("download", this.selected + ".csv");
        link.innerHTML= "";
        document.body.appendChild(link); // Required for FF

        link.click(); // This will download the data file named "my_data.csv".
      }
      catch (err)
      {
        console.error(err);
      }
    }
  }


  updateSelected = (newSelection) =>
  {
    this.selected = newSelection;
    this.setState({ selected: newSelection });
    this.getData();
  }

  render()
  {
    const { batchData } = this.state;
    const { selected } = this.state;

    var batchNamesJSON = [];
    var i = 0;
    for (let name of this.state.batchNames)
    {
      batchNamesJSON.push({value: i, label: name});
      i++;
    }

    return (
      <div className="App">
      {
        <div>
          <h1>Mead Fermentation Temperature Monitoring</h1>
          <Select
            id="batch-select"
            ref={(ref) => { this.select = ref; }}
            onBlurResetsInput={false}
            onSelectResetsInput={false}
            autoFocus
            options={ batchNamesJSON }
            simpleValue
            clearable={false}
            name="selected-batch"
            disabled={false}
            value={{label: selected}}
            onChange=
            {(val) =>
              {
                if (val !== null)
                {
                  this.updateSelected(batchNamesJSON[val].label);
                }
              }
            }
            searchable={true}
            rtl={false}
          />

          <br></br>
          <button
            className = "CSV-button"
            onClick={this.printCSV}
          >
            {'Export to CSV'}
          </button>

          <LineChart
            data={[
              batchData
            ]}
            datePattern={'%Y-%m-%d-%H:%M'}
            xType={'time'}
            xTicks={20}
            axes
            grid
            verticalGrid
            yDomainRange={[50, 90]}
            axisLabels={{x: 'Time', y: 'Temperature (F)'}}
            height={600}
            width={900}
          />
        </div>
      }
      </div>
    );
  }
}

export default App;