fetch('/memory_usage')
.then(response => response.json())
.then();
fetch('/cpu_usage')
.then(response => response.json())
.then();
fetch('/net_usage')
.then(response => response.json())
.then();
let cpu_chart =new Chart(document.getElementById("cpu-chart"), {
    type: 'pie',
    data: {
    labels: ["System Usage", "User Usage", "Idle"],
    datasets: [{ 
        label: "CPU Usage",
        data: [0.0,0.0,0.0],
        backgroundColor: ['#ffd950', '#02bc77', '#28c3d7'],
        borderColor: "#22252B",
        fill: false
      }]
    },

    options: {
      responsive: false,
      maintainAspectRatio: false, 
      title: {
          display:true,
          text: 'CPU Usage',
          fontSize:25,
          fontColor: 'black',
      },
      legend: {
          position: 'bottom',
          fontColor: 'black',
          align: 'center',
          display: true,
          fullWidth: true,
          labels: {
              fontSize:15,
              fontColor: 'rgb(0, 0, 0)'
          }
      },
    }
  });
  let memory_chart =new Chart(document.getElementById("memory-chart"), {
    type: 'pie',
    data: {
    labels: ["Usage", "Free"],
    datasets: [{ 
        label: "Memory Usage",
        data: [0.0,0.0],
        backgroundColor: ['#ffd950', '#02bc77'],
        borderColor: "#22252B",
        fill: false
      }]
    },

    options: {
      responsive: false,
      maintainAspectRatio: false, 
      title: {
          display:true,
          text: 'Memory Usage',
          fontSize:25,
          fontColor: 'black',
      },
      legend: {
          position: 'bottom',
          fontColor: 'black',
          align: 'center',
          display: true,
          fullWidth: true,
          labels: {
              fontSize:15,
              fontColor: 'rgb(0, 0, 0)'
          }
      },
    }
  });
  let rec_net_chart =new Chart(document.getElementById("recv-chart"), {
    type: 'line', // 차트의 형태
    data: { // 차트에 들어갈 데이터
        labels:[0,1,2,3,4,5,6,7,8],
        datasets: [
          { 
            data: [0],
            label: "Received(kb/sec)",
            borderColor: "#3e95cd",
            fill: false
          }
        ]
    },
    options: {
      responsive: false,
        scales: {
            yAxes: [
                {
                    ticks: {
                        beginAtZero: true
                    }
                }
            ]
        }
    }
});
let tra_net_chart =new Chart(document.getElementById("trans-chart"), {
  type: 'line', 
  data: { 
      labels:[0,1,2,3,4,5,6,7,8],
      datasets: [
           { 
          data: [0],
          label: "Transmitted(kb/sec)",
          borderColor: "#8e5ea2",
          fill: false
        }
      ]
  },
  options: {
    responsive: false,
      scales: {
          yAxes: [
              {
                  ticks: {
                      beginAtZero: true
                  }
              }
          ]
      }
  }
});

    setInterval(
        function () { 
          fetch('/memory_usage')
          .then(response => response.json())
          .then(
            function(json_value){
            memory_chart.data.datasets[0].data[0] = json_value.MemoryUsagePercent;
            memory_chart.data.datasets[0].data[1] = 100.0 - json_value.MemoryUsagePercent;
            memory_chart.update();
          }); 

          fetch('/cpu_usage')
          .then(response => response.json())
          .then(function(json_value){
            cpu_chart.data.datasets[0].data[0] = json_value.SystemUsagePercent;
            cpu_chart.data.datasets[0].data[1] = json_value.UserUsagePercent;
            cpu_chart.data.datasets[0].data[2] = json_value.IdlePercent;
            cpu_chart.update();
          });
          
          fetch('/net_usage')
          .then(response => response.json())
          .then(function(json_value){
            rec_net_chart.data.datasets[0].data.push(json_value.RxByteSum);
            if(rec_net_chart.data.datasets[0].data.length >= 10)
            {
              rec_net_chart.data.datasets[0].data.shift();
            }

            tra_net_chart.data.datasets[0].data.push(json_value.TxByteSum);
            if(tra_net_chart.data.datasets[0].data.length >= 10)
            {
              tra_net_chart.data.datasets[0].data.shift();
            }
            tra_net_chart.update();
            rec_net_chart.update();
            }
          ); 
        }
    , 1000);