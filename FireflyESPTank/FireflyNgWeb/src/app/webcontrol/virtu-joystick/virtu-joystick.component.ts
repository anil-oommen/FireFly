import { Component, OnInit,OnDestroy,AfterViewInit } from '@angular/core';
import {WebmqttService} from "../../webmqtt.service";
import * as createjs from 'createjs-module';
import {McuCommand,McuFeedback} from "../../../_models/index";

@Component({
  selector: 'app-virtu-joystick',
  templateUrl: './virtu-joystick.component.html',
  styleUrls: ['./virtu-joystick.component.css']
})
export class VirtuJoystickComponent implements OnInit,  AfterViewInit {

  public dcMotorADirection: boolean;
  public mcuCommand: string;
  public mcuFeedback: string;
  public mcuSignature : string ;
  public debugLine1: string;
  public debugLine2: string;
  public connectToggle: boolean;

  constructor(public webmqtt:WebmqttService) { }

  ngOnInit() {
  }

  xCoordinate : number=0;
  yCoordinate : number=0;
  xCenter : number= 150;
  yCenter : number= 150;
  stage :any ;
  psp :any;

  public ngAfterViewInit() {
    this.stage = new createjs.Stage('joystick');
  
    this.psp = new createjs.Shape();
    this.psp.graphics.beginFill('#060e16')
        .drawCircle(this.xCenter, this.yCenter, 50);
  
    this.psp.alpha = 0.25;
  
    var vertical = new createjs.Shape();
    var horizontal = new createjs.Shape();
    vertical.graphics.beginFill('#4b6fa0').drawRect(150, 0, 2, 300);
    horizontal.graphics.beginFill('#4b6fa0').drawRect(0, 150, 300, 2);
  
    
    this.stage.addChild(vertical);
    this.stage.addChild(horizontal);
    this.stage.addChild(this.psp);
    createjs.Ticker.framerate = 60;
    createjs.Ticker.addEventListener('tick', this.stage);
    this.stage.update();

    
    this.webmqtt.getMCUFeedbackObserve().subscribe(
      (feedback:McuFeedback)=>{
        if(feedback.event == "Bootup"){
          this.mcuSignature =  feedback.value + " " +  feedback.message  ; 
        }
        this.mcuFeedback = JSON.stringify(feedback);
      }
    )

    this.webmqtt.getMCUCommandObserve().subscribe(
      (command:McuCommand)=>{
        this.mcuCommand = JSON.stringify(command);
      }
    )

  }

  public onPanStart(event:any){
    this.xCenter = this.psp.x;
    this.yCenter = this.psp.y;
    this.psp.alpha = 0.5;
    this.stage.update();
  }

  public onPanMove(event:any){
    var pos = document.getElementById('joystick').getBoundingClientRect();
    var x = (event.center.x - pos.left - 150);
    var y = (event.center.y - pos.top - 150);

    /* Old Computation
    this.xCoordinate = Math.round(x/10);
    this.yCoordinate = Math.round(y/10);
    */
    this.xCoordinate = Math.round(this.psp.x/10);
    this.yCoordinate = Math.round(this.psp.y/10);

    this.yCoordinate = this.yCoordinate * -1;

    //this.mcuFeedback = "> " + Math.round(this.psp.x/10) + "   " + Math.round(this.psp.y/10); 

    this.calculateCoords(event.angle, event.distance);
    this.psp.alpha = 0.5;
    
    this.stage.update();

    //var mcuCommand = new McuCommand();
    //mcuCommand.joystickXY(this.xCoordinate, this.yCoordinate);
    //this.webmqtt.sendMCURunCommand(mcuCommand);

    var displacement = Math.round(Math.sqrt((this.xCoordinate*this.xCoordinate) + (this.yCoordinate*this.yCoordinate)));
    var motorADirection = this.yCoordinate==0?1:((Math.sqrt(this.yCoordinate * this.yCoordinate))/this.yCoordinate);
    var motorBDirection = motorADirection;

    var mcuCommand = new McuCommand();
    if(this.xCoordinate > 0 && this.yCoordinate > 0){
      //First Quadrant.
      motorADirection = 1;  motorBDirection=1;
      var motorBPWMSignal = (300*(displacement/10) + 700); // Speed based on displacement.
      var motorAPWMSignal = (300*(displacement/10)*(this.yCoordinate/10) + 700); 
      this.debugLine2 = "Q1 [>>>] PWM A,B  " + motorAPWMSignal + " , " + motorBPWMSignal;
      mcuCommand.dcMotorSignal(motorADirection,motorAPWMSignal,motorBDirection,motorBPWMSignal);
    }else if(this.xCoordinate < 0  && this.yCoordinate > 0){
      //Last Quadrant.
      motorADirection = 1;  motorBDirection=1;
      var motorBPWMSignal = (300*(displacement/10)*(this.yCoordinate/10) + 700); 
      var motorAPWMSignal = (300*(displacement/10) + 700); 
      this.debugLine2 = "Q4 [>>>] PWM A,B  " + motorAPWMSignal + " , " + motorBPWMSignal;
      mcuCommand.dcMotorSignal(motorADirection,motorAPWMSignal,motorBDirection,motorBPWMSignal);
    }else if(this.xCoordinate > 0 && this.yCoordinate < 0 ){
      //Second Quadrant
      motorADirection = 0;  motorBDirection=0;
      var motorBPWMSignal = (300*(displacement/10) + 700); // Speed based on displacement.
      var motorAPWMSignal = (300*(displacement/10)*((this.yCoordinate*-1)/10) + 700); 
      this.debugLine2 = "Q2 [<<<] PWM A,B  " + motorAPWMSignal + " , " + motorBPWMSignal;
      mcuCommand.dcMotorSignal(motorADirection,motorAPWMSignal,motorBDirection,motorBPWMSignal);
    }else if(this.xCoordinate <0 && this.yCoordinate < 0 ){
      //Third Quadrant
      motorADirection = 0;  motorBDirection=0;
      var motorBPWMSignal = (300*(displacement/10)*((this.yCoordinate*-1)/10) + 700); 
      var motorAPWMSignal = (300*(displacement/10) + 700); 
      this.debugLine2 = "Q3 [<<<] PWM A,B  " + motorAPWMSignal + " , " + motorBPWMSignal;
      mcuCommand.dcMotorSignal(motorADirection,motorAPWMSignal,motorBDirection,motorBPWMSignal);
    }
    this.webmqtt.sendMCURunCommand(mcuCommand);

    //this.debugLine1  = "Distance:" + displacement  + " x.y "+ this.xCoordinate +" " +  this.yCoordinate
    //this.debugLine2  = " direction-> x:" + motorADirection + " y:" + motorBDirection
    //this.mcuFeedback = "Direction:" + this.yCoordinate +"    " +  (Math.sqrt(this.yCoordinate * this.yCoordinate))/this.yCoordinate ;
    
  }

  public onPanEnd(event:any){
    this.psp.alpha = 0.25;
    createjs.Tween.get(this.psp).to({x:this.xCenter,y:this.yCenter},750,createjs.Ease.elasticOut);
    this.xCoordinate = 0;
    this.yCoordinate = 0;
    var mcuCommand = new McuCommand();
    mcuCommand.stop();
    this.webmqtt.sendMCURunCommand(mcuCommand);
  }



  calculateCoords(angle, distance) {
    distance = Math.min(distance, 100);  
    var rads = (angle * Math.PI) / 180.0;
  
    this.psp.x = distance * Math.cos(rads);
    this.psp.y = distance * Math.sin(rads);
    return ;
  }

}
